#include "PmConfig.h"
#include "IFileSysUtil.h"
#include <json/json.h>
#include "PmLogger.h"
#include "RandomUtil.h"
#include "IUcidAdapter.h"
#include "PmTypes.h"

PmConfig::PmConfig( IFileSysUtil& fileUtil, IUcidAdapter& ucidAdapter )
    : m_fileUtil( fileUtil )
    , m_ucidAdapter( ucidAdapter )
    , m_isFirstCheckin( true )
    , m_configData( { 0 } )
{
}

PmConfig::~PmConfig()
{
}

int32_t PmConfig::LoadPmConfig( const std::string& pmConfig )
{
    int rtn = -1;
    std::lock_guard<std::mutex> lock( m_mutex );

    std::string pmData = m_fileUtil.ReadFile( pmConfig );
    m_pmConfigFileTimestamp = m_fileUtil.FileTime( pmConfig );

    rtn = ParsePmConfig( pmData );

    if( rtn != 0 ) {
        LOG_ERROR( "Failed to parse %s", pmConfig.c_str() );
        pmData = m_fileUtil.ReadFile( pmConfig + ".bak" );

        rtn = ParsePmConfig( pmData );

        if( rtn == 0 ) {
            //replace current file with backup?
        }
        else {
            LOG_ERROR( "Failed to parse %s", ( pmConfig + ".bak" ).c_str() );

            m_configData.intervalMs = PM_CONFIG_INTERVAL_DEFAULT_MS;
            m_configData.maxStartupDelayMs = PM_CONFIG_MAX_STARTUP_DELAY_DEFAULT_MS;
            m_configData.log_level = PM_CONFIG_LOGLEVEL_DEFAULT;
            m_configData.maxFileCacheAge = PM_CONFIG_MAX_CACHE_AGE_DEFAULT_SECS;
            m_configData.allowPostInstallReboots = false;
            m_configData.rebootThrottleS = PM_CONFIG_REBOOT_THROTTLE_DEFAULT_SECS;
            m_configData.watchdogTimeoutMs = PM_CONFIG_INTERVAL_DEFAULT_MS + PM_CONFIG_WATCHDOG_BUFFER_DEFAULT_MS;
            m_configData.maxEventTtl = PM_CONFIG_MAX_EVENT_TTL_SECS;
        }
    }

    GetPMLogger()->SetLogLevel( ( IPMLogger::Severity )m_configData.log_level );

    return rtn;
}

bool PmConfig::PmConfigFileChanged( const std::string& pmConfig )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_pmConfigFileTimestamp != m_fileUtil.FileTime( pmConfig );
}

std::string PmConfig::GetCloudCheckinUri()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    PmUrlList urls;
    return  m_ucidAdapter.GetUrls( urls ) ? urls.checkinUrl : "";
}

std::string PmConfig::GetCloudEventUri()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    PmUrlList urls;
    return  m_ucidAdapter.GetUrls( urls ) ? urls.eventUrl + EVENT_URL_VERSION : "";
}

std::string PmConfig::GetCloudCatalogUri()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    PmUrlList urls;
    return  m_ucidAdapter.GetUrls( urls ) ? urls.catalogUrl : "";
}

uint32_t PmConfig::GetCloudCheckinIntervalMs()
{
    std::lock_guard<std::mutex> lock( m_mutex );
    uint32_t retval = m_configData.intervalMs;
    if( m_isFirstCheckin )
    {
        m_isFirstCheckin = false;
        retval = RandomUtil::GetInt( 2000, m_configData.maxStartupDelayMs );
        LOG_DEBUG( "Random first time checkin delay: %d", retval );
    }

    return retval;
}

uint32_t PmConfig::GetLogLevel()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.log_level;
}

uint32_t PmConfig::GetMaxFileCacheAgeS()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.maxFileCacheAge;
}

uint32_t PmConfig::GetMaxEventTtlS()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.maxEventTtl;
}

uint32_t PmConfig::GetRebootThrottleS()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.rebootThrottleS;
}

uint32_t PmConfig::GetWatchdogTimeoutMs()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.watchdogTimeoutMs;
}

uint32_t PmConfig::GetNetworkFailureRetryInterval()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.networkFailureRetryInterval;
}

int32_t PmConfig::ParsePmConfig( const std::string& pmConfig )
{
    int rtn = -1;

    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root, pm;
    std::string jsonError;

    if( pmConfig.empty() ) {
        LOG_ERROR( "config contents is empty" );
    }
    else if( !jsonReader->parse( pmConfig.c_str(), pmConfig.c_str() + pmConfig.length(), &root, &jsonError ) ) {
        LOG_ERROR( "Json Parse error %s", jsonError.c_str() );
    }
    else {
        pm = root[ "pm" ];

        if ( !VerifyPmLogLevel( pm ) ) {
            LOG_WARNING("Invalid loglevel. Using default");
            m_configData.log_level = PM_CONFIG_LOGLEVEL_DEFAULT;
        }
        else {
            m_configData.log_level = pm["loglevel"].asUInt();
        }

        if ( !VerifyPmCheckinInterval( pm ) ) {
            LOG_WARNING("Invalid CheckinInterval. Using default");
            m_configData.intervalMs = PM_CONFIG_INTERVAL_DEFAULT_MS;
        }
        else {
            m_configData.intervalMs = pm["CheckinInterval"].asUInt();
        }


        if ( !VerifyPmMaxStartupDelay( pm ) ) {
            LOG_WARNING("Invalid MaxStartupDelay. Using default");
            m_configData.maxStartupDelayMs = PM_CONFIG_MAX_STARTUP_DELAY_DEFAULT_MS;
        }
        else {
            m_configData.maxStartupDelayMs = pm["MaxStartupDelay"].asUInt();
        }

        if ( !VerifyPmAllowPostInstallReboots( pm ) ) {
            LOG_WARNING("Invalid AllowPostInstallReboots. Using default");
            m_configData.allowPostInstallReboots = false;
        }
        else {
            m_configData.allowPostInstallReboots = pm["AllowPostInstallReboots"].asBool();
        }

        if ( !VerifyPmMaxFileCacheAge( pm ) ) {
            LOG_WARNING("Invalid maxFileCacheAge_s. Using default");
            m_configData.maxFileCacheAge = PM_CONFIG_MAX_CACHE_AGE_DEFAULT_SECS;
        }
        else {
            m_configData.maxFileCacheAge = pm["maxFileCacheAge_s"].asUInt();
        }

        if( !VerifyPmMaxEventTtl( pm ) ) {
            LOG_WARNING( "Invalid MaxEventTTL_s. Using default" );
            m_configData.maxEventTtl = PM_CONFIG_MAX_EVENT_TTL_SECS;
        }
        else {
            m_configData.maxEventTtl = pm[ "MaxEventTTL_s" ].asUInt();
        }

        if( !VerifyPmRebootThrottle( pm ) ) {
            LOG_WARNING( "Invalid RebootThrottleS. Using default" );
            m_configData.rebootThrottleS = PM_CONFIG_REBOOT_THROTTLE_DEFAULT_SECS;
        }
        else {
            m_configData.rebootThrottleS = pm[ "RebootThrottleS" ].asUInt();
        }

        if( !VerifyPmWatchdogBuffer( pm ) ) {
            LOG_WARNING( "Invalid WatchdogInterval. Using default" );
            m_configData.watchdogTimeoutMs = m_configData.intervalMs + PM_CONFIG_WATCHDOG_BUFFER_DEFAULT_MS;
        }
        else {
            m_configData.watchdogTimeoutMs = m_configData.intervalMs + pm[ "WatchdogBufferMs" ].asUInt();
        }

        if ( !VerifyPmNetworkFailureRetryInterval( pm ) ) {
            uint32_t networkFailureRetry = m_configData.intervalMs / 2;
            LOG_WARNING( "Invalid NetworkFailureRetryInterval. Using %d", networkFailureRetry );
            m_configData.networkFailureRetryInterval = networkFailureRetry;
        }
        else {
            m_configData.networkFailureRetryInterval = pm[ "NetworkFailureRetryInterval" ].asUInt();
        }

        rtn = 0;
    }

    return rtn;
}

int32_t PmConfig::VerifyPmContents( const std::string& pmData )
{
    int32_t rtn = 0;

    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root, pm;
    std::string jsonError;

    if( pmData.empty() ) {
        LOG_ERROR( "config contents is empty" );
        rtn = -1;
    }
    else if( !jsonReader->parse( pmData.c_str(), pmData.c_str() + pmData.length(), &root, &jsonError ) ) {
        LOG_ERROR( "Json Parse error %s", jsonError.c_str() );
        rtn = -1;
    }
    else {
        pm = root[ "pm" ];

        
        if( !VerifyPmLogLevel( pm ) ) {
            LOG_ERROR( "Invalid loglevel" );
            rtn = -1;
        }

        if( !VerifyPmCheckinInterval( pm ) ) {
            LOG_ERROR( "Invalid CheckinInterval" );
            rtn = -1;
        }

        if( !VerifyPmMaxStartupDelay( pm ) ) {
            LOG_ERROR( "Invalid MaxStartupDelay" );
            rtn = -1;
        }
        
        if ( !VerifyPmMaxFileCacheAge( pm ) ) {
            LOG_ERROR( "Invalid maxFileCacheAge_s" );
            rtn = -1;
        }

        if( !VerifyPmMaxEventTtl( pm ) ) {
            LOG_ERROR( "Invalid MaxEventTTL_s" );
            rtn = -1;
        }

        if( !VerifyPmAllowPostInstallReboots( pm ) ) {
            LOG_ERROR( "Invalid AllowPostInstallReboots" );
            rtn = -1;
        }

        if( !VerifyPmRebootThrottle( pm ) ) {
            LOG_ERROR( "Invalid RebootThrottle" );
            rtn = -1;
        }

        if( !VerifyPmWatchdogBuffer( pm ) ) {
            LOG_ERROR( "Invalid WatchdogInterval" );
            rtn = -1;
        }

        if ( !VerifyPmNetworkFailureRetryInterval( pm ) ) {
            LOG_ERROR( "Invalid NetworkFailureRetryInterval" );
            rtn = -1;
        }

        if( rtn != 0 ) {
            LOG_ERROR( "Invalid configuration %s", Json::writeString( Json::StreamWriterBuilder(), root ).c_str() );
        }
    }

    return rtn;
}

int32_t PmConfig::VerifyPmFileIntegrity( const std::string& pmConfig )
{
    std::string pmData = m_fileUtil.ReadFile( pmConfig );

    return VerifyPmContents( pmData );
}

bool PmConfig::AllowPostInstallReboots()
{
    return m_configData.allowPostInstallReboots;
}

bool PmConfig::VerifyPmLogLevel( const Json::Value& pmRoot )
{
    return pmRoot.isMember( "loglevel" ) && pmRoot[ "loglevel" ].isUInt() && ( pmRoot[ "loglevel" ].asUInt() <= IPMLogger::LOG_DEBUG );
}

bool PmConfig::VerifyPmCheckinInterval( const Json::Value& pmRoot )
{
    return pmRoot.isMember( "CheckinInterval" ) && pmRoot[ "CheckinInterval" ].isUInt() && ( pmRoot[ "CheckinInterval" ].asUInt() >= 2000 );
}

bool PmConfig::VerifyPmMaxStartupDelay( const Json::Value& pmRoot )
{
    return pmRoot.isMember( "MaxStartupDelay" ) && pmRoot[ "MaxStartupDelay" ].isUInt() && ( pmRoot[ "MaxStartupDelay" ].asUInt() >= 2000 );
}

bool PmConfig::VerifyPmMaxFileCacheAge( const Json::Value& pmRoot )
{
    return pmRoot.isMember( "maxFileCacheAge_s" ) && pmRoot[ "maxFileCacheAge_s" ].isUInt();
}

bool PmConfig::VerifyPmMaxEventTtl( const Json::Value& pmRoot )
{
    return pmRoot.isMember( "MaxEventTTL_s" ) && pmRoot[ "MaxEventTTL_s" ].isUInt();
}

bool PmConfig::VerifyPmAllowPostInstallReboots( const Json::Value& pmRoot )
{
    return pmRoot.isMember( "AllowPostInstallReboots" ) && pmRoot[ "AllowPostInstallReboots" ].isBool();
}

bool PmConfig::VerifyPmRebootThrottle( const Json::Value& pmRoot )
{
    return pmRoot.isMember( "RebootThrottleS" ) && pmRoot[ "RebootThrottleS" ].isUInt() && ( pmRoot[ "RebootThrottleS" ].asUInt() >= 60 );
}

bool PmConfig::VerifyPmWatchdogBuffer( const Json::Value& pmRoot )
{
    return pmRoot.isMember( "WatchdogBufferMs" ) && pmRoot[ "WatchdogBufferMs" ].isUInt() && ( pmRoot[ "WatchdogBufferMs" ].asUInt() >= 30000 );
}

bool PmConfig::VerifyPmNetworkFailureRetryInterval( const Json::Value& pmRoot )
{
    return pmRoot.isMember( "NetworkFailureRetryInterval" ) && pmRoot[ "NetworkFailureRetryInterval" ].isUInt();
}

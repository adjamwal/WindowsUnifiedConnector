#include "PmConfig.h"
#include "PmTypes.h"
#include "IFileUtil.h"
#include <json/json.h>
#include "PmLogger.h"
#include "RandomUtil.h"

PmConfig::PmConfig( IFileUtil& fileUtil )
    : m_fileUtil( fileUtil )
    , m_isFirstCheckin( true )
{
}

PmConfig::~PmConfig()
{
}

int32_t PmConfig::LoadBsConfig( const std::string& bsConfig )
{
    int rtn = -1;
    std::lock_guard<std::mutex> lock( m_mutex );

    std::string bsData = m_fileUtil.ReadFile( bsConfig );

    rtn = ParseBsConfig( bsData );

    if( rtn != 0 ) {
        LOG_ERROR( "Failed to parse %s", bsConfig.c_str() );
        bsData = m_fileUtil.ReadFile( bsConfig + ".bak" );

        rtn = ParseBsConfig( bsData );

        if( rtn == 0 ) {
            //replace current file with backup?
        }
        else {
            LOG_ERROR( "Failed to parse %s", ( bsConfig + ".bak" ).c_str() );
        }
    }

    return rtn;
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

            m_configData.intervalMs = PM_CONFIG_INTERVAL_DEFAULT;
            m_configData.maxDelayMs = PM_CONFIG_INTERVAL_DEFAULT;
            m_configData.log_level = PM_CONFIG_LOGLEVEL_DEFAULT;
            m_configData.maxFileCacheAge = PM_CONFIG_MAX_CACHE_AGE_DEFAULT_SECS;
            m_configData.allowPostInstallReboots = false;
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

const std::string& PmConfig::GetCloudIdentifyUri()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.identifyUri;
}

const std::string& PmConfig::GetCloudCheckinUri()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.checkinUri;
}

const std::string& PmConfig::GetCloudEventUri()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.eventUri;
}

const std::string& PmConfig::GetCloudCatalogUri()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.catalogUri;
}

uint32_t PmConfig::GetCloudCheckinIntervalMs()
{
    std::lock_guard<std::mutex> lock( m_mutex );
    uint32_t retval = m_configData.intervalMs;
    if( m_isFirstCheckin )
    {
        m_isFirstCheckin = false;
        retval = RandomUtil::GetInt( 2000, m_configData.maxDelayMs );
        LOG_DEBUG( "Random first time checkin delay: %d", retval );
    }

    return retval;
}

uint32_t PmConfig::GetLogLevel()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.log_level;
}

const std::vector<PmComponent>& PmConfig::GetSupportedComponentList()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_ComponentList;
}

uint32_t PmConfig::GetMaxFileCacheAge()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.maxFileCacheAge;
}

int32_t PmConfig::ParseBsConfig( const std::string& bsConfig )
{
    int rtn = -1;

    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root, pm, id;
    std::string jsonError;

    if( bsConfig.empty() ) {
        LOG_ERROR( "Config contents is empty" );
    }
    else if( !jsonReader->parse( bsConfig.c_str(), bsConfig.c_str() + bsConfig.length(), &root, &jsonError ) ) {
        LOG_ERROR( "Json Parse error %s", jsonError.c_str() );
    }
    else if( VerifyBsContents( bsConfig ) != 0 ) {
        LOG_ERROR( "Failed to verify config contents" );
    }
    else {
        id = root[ "id" ];
        m_configData.identifyUri = id[ "url" ].asString();

        pm = root[ "pm" ];
        m_configData.checkinUri = pm[ "url" ].asString();
        m_configData.eventUri = pm[ "event_url" ].asString();
        m_configData.catalogUri = pm[ "catalog_url" ].asString();

        rtn = 0;
    }

    return rtn;
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
    else if( VerifyPmContents( pmConfig ) != 0 ) {
        LOG_ERROR( "Failed to verify config contents" );
    }
    else {
        pm = root[ "pm" ];
        m_configData.log_level = pm[ "loglevel" ].asUInt();
        m_configData.intervalMs = pm[ "CheckinInterval" ].asUInt();
        m_configData.maxDelayMs = pm[ "MaxStartupDelay" ].asUInt();
        m_configData.allowPostInstallReboots = pm[ "AllowPostInstallReboots" ].asBool();

        //optional fields
        if ( pm.isMember( "maxFileCacheAge_s" ) ) {
            m_configData.maxFileCacheAge = pm[ "maxFileCacheAge_s" ].asUInt();
        }
        else {
            m_configData.maxFileCacheAge = PM_CONFIG_MAX_CACHE_AGE_DEFAULT_SECS;
        }

        rtn = 0;
    }

    return rtn;
}

int32_t PmConfig::VerifyBsContents( const std::string& bsData )
{
    int32_t rtn = 0;

    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root, pm, id;
    std::string jsonError;

    if( bsData.empty() ) {
        LOG_ERROR( "config contents is empty" );
        rtn = -1;
    }
    else if( !jsonReader->parse( bsData.c_str(), bsData.c_str() + bsData.length(), &root, &jsonError ) ) {
        LOG_ERROR( "Json Parse error %s", jsonError.c_str() );
        rtn = -1;
    }
    else {
        id = root[ "id" ];
        if( !id[ "url" ].isString() ) {
            LOG_ERROR( "Invalid Identify Url" );
            rtn = -1;
        }

        pm = root[ "pm" ];
        if( !pm[ "url" ].isString() ) {
            LOG_ERROR( "Invalid Checkin Url" );
            rtn = -1;
        }

        if( !pm[ "event_url" ].isString() ) {
            LOG_ERROR( "Invalid Event Url" );
            rtn = -1;
        }

        if( !pm[ "catalog_url" ].isString() ) {
            LOG_ERROR( "Invalid Catalog Url" );
            rtn = -1;
        }

        if( rtn != 0 ) {
            LOG_ERROR( "Invalid configuration %s", Json::writeString( Json::StreamWriterBuilder(), root ).c_str() );
        }
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

        if( !pm[ "loglevel" ].isUInt() ) {
            LOG_ERROR( "Invalid loglevel" );
            rtn = -1;
        }

        if( !pm[ "CheckinInterval" ].isUInt() ) {
            LOG_ERROR( "Invalid CheckinInterval" );
            rtn = -1;
        }
        else if( pm[ "CheckinInterval" ].asUInt() < 2000 ) {
            LOG_ERROR( "CheckinInterval cannot be less than 2000 ms" );
            rtn = -1;
        }

        if( !pm[ "MaxStartupDelay" ].isUInt() ) {
            LOG_ERROR( "Invalid MaxStartupDelay" );
            rtn = -1;
        }
        else if( pm[ "MaxStartupDelay" ].asUInt() < 2000 ) {
            LOG_ERROR( "MaxStartupDelay cannot be less than 2000 ms" );
            rtn = -1;
        }
        
        if ( pm.isMember( "maxFileCacheAge_s" ) && !pm[ "maxFileCacheAge_s" ].isUInt() ) {
            LOG_ERROR( "Invalid maxFileCacheAge_s" );
            rtn = -1;
        }

        if( !pm[ "AllowPostInstallReboots" ].isBool() ) {
            LOG_ERROR( "Invalid AllowPostInstallReboots" );
            rtn = -1;
        }

        if( rtn != 0 ) {
            LOG_ERROR( "Invalid configuration %s", Json::writeString( Json::StreamWriterBuilder(), root ).c_str() );
        }
    }

    return rtn;
}

int32_t PmConfig::VerifyBsFileIntegrity( const std::string& bsConfig )
{
    std::string bsData = m_fileUtil.ReadFile( bsConfig );

    return VerifyBsContents( bsData );

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

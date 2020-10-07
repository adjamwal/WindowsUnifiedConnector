#include "PmConfig.h"
#include "PmTypes.h"
#include "IFileUtil.h"
#include "json/json.h"
#include "PmLogger.h"

PmConfig::PmConfig( IFileUtil& fileUtil ) :
    m_fileUtil( fileUtil )
{
}

PmConfig::~PmConfig()
{
}

int32_t PmConfig::Load( const std::string& filename )
{
    int rtn = -1;
    std::lock_guard<std::mutex> lock( m_mutex );
    
    std::string config = m_fileUtil.ReadFile( filename );

    rtn = ParseConfig( config );

    if( rtn != 0 ) {
        LOG_ERROR( "Failed to parse %s", filename.c_str() );
        config = m_fileUtil.ReadFile( filename + ".bak" );

        rtn = ParseConfig( config );

        if( rtn == 0 ) {
            //replace current file with backup?
        }
        else {
            LOG_ERROR( "Failed to parse %s", ( filename + ".bak" ).c_str() );
        }
    }

    return rtn;
}

const std::string& PmConfig::GetCloudUri()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.cloudUri;
}

uint32_t PmConfig::GetCloudInterval()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_configData.interval;
}

const std::vector<PmComponent>& PmConfig::GetSupportedComponentList()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_ComponentList;
}

int32_t PmConfig::VerifyContents( const std::string& config )
{
    int32_t rtn = 0;

    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root, cloud;
    std::string jsonError;

    if( config.empty() ) {
        LOG_ERROR( "config contents is empty" );
        rtn = -1;
    }
    else if( !jsonReader->parse( config.c_str(), config.c_str() + config.length(), &root, &jsonError ) ) {
        LOG_ERROR( "Json Parse error %s", jsonError.c_str() );
        rtn = -1;
    }
    else {
        cloud = root[ "cloud" ];
        if( !cloud[ "CheckinUri" ].isString() ) {
            LOG_ERROR( "Invalid CheckinUrl" );
            rtn = -1;
        }

        if( !cloud[ "CheckinInterval" ].isUInt() ) {
            LOG_ERROR( "Invalid CheckinInterval" );
            rtn = -1;
        }
        else if( cloud[ "CheckinInterval" ].asUInt() == 0 ) {
            LOG_ERROR( "CheckinInterval cannot be 0" );
            rtn = -1;
        }

        if( rtn != 0 ) {
            LOG_ERROR( "Invalid configuartion %s", Json::writeString( Json::StreamWriterBuilder(), root ).c_str() );
        }
    }

    return rtn;
}

int32_t PmConfig::ParseConfig( const std::string& config )
{
    int rtn = -1;

    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root, cloud;
    std::string jsonError;

    if( config.empty() ) {
        LOG_ERROR( "config contents is empty" );
    }
    else if( !jsonReader->parse( config.c_str(), config.c_str() + config.length(), &root, &jsonError ) ) {
        LOG_ERROR( "Json Parse error %s", jsonError.c_str() );
    }
    else if( VerifyContents( config ) != 0) {
        LOG_ERROR( "Failed to verify config contents" );
    }
    else {
        cloud = root[ "cloud" ];
        m_configData.cloudUri = cloud[ "CheckinUri" ].asString();
        m_configData.interval = cloud[ "CheckinInterval" ].asUInt();

        rtn = 0;
    }

    return rtn;
}

int32_t PmConfig::VerifyFileIntegrity( const std::string& filename )
{
    std::string config = m_fileUtil.ReadFile( filename );

    return VerifyContents( config );
    
}
#include "pch.h"
#include "framework.h"
#include "UcConfig.h"
#include "IUcLogger.h"
#include "WindowsUtilities.h"
#include "json\json.h"

UcConfig::UcConfig() :
    m_logLevel( ( uint32_t )DEFAULT_LOG_LEVEL )
    , m_fileModifyTime( 0 )
{
    if( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, L"Software\\Cisco\\SecureXYZ\\UnifiedConnector\\config", L"Service", m_path ) )
    {
        WLOG_ERROR( L"Failed to read UnifiedConnector config path from registry" );
    }
}

UcConfig::~UcConfig()
{

}

bool UcConfig::LoadConfig()
{
    std::lock_guard<std::mutex> lock( m_mutex );
    bool rtn = false;

    try {
        if( !WindowsUtilities::FileExists( m_path.c_str() ) ) {
            throw( std::exception( __FUNCTION__ ": config file is missing" ) );
        }

        uint32_t modifyTime = WindowsUtilities::GetFileModifyTime( m_path.c_str() );

        if( modifyTime > m_fileModifyTime ) {
            m_fileModifyTime = modifyTime;
            Json::Value root = GetJsonFromFile( m_path );

            m_logLevel = root[ "uc" ][ "loglevel" ].asInt();
            rtn = true;
        }
    }
    catch( std::exception ex ) {
        LOG_WARNING( "LoadConfig Failed: %s", ex.what() );
        LOG_WARNING( "Restoring default configs ");
        m_logLevel = ( uint32_t )DEFAULT_LOG_LEVEL;
        m_fileModifyTime = 0;
    }

    return rtn;
}

bool UcConfig::VerifyConfig( const std::wstring& path )
{
    bool rtn = false;
    const std::wstring& verifyPath = !path.empty() ? path : m_path;

    try {
        Json::Value root = GetJsonFromFile( verifyPath );

        rtn = true;
    }
    catch( std::exception ex ) {
        LOG_ERROR( "LoadConfig Failed: %s", ex.what() );
    }

    return rtn;
}

Json::Value UcConfig::GetJsonFromFile( const std::wstring& path )
{
    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    std::string contents = WindowsUtilities::ReadFileContents( path.c_str() );
    Json::Value root;
    std::string jsonError;

    if( contents.empty() ) {
        throw( std::exception( __FUNCTION__ ": file contents is empty" ) );
    }
    else if( !jsonReader->parse( contents.c_str(), contents.c_str() + contents.length(), &root, &jsonError ) ) {
        jsonError = std::string( __FUNCTION__ ) + ": " + jsonError;
        throw( std::exception(  jsonError.c_str()  ) );
    }
    else if( !root.isMember( "uc" ) ) {
        throw( std::exception( __FUNCTION__ ": missing uc_service element" ) );
    }
    else if( !root[ "uc" ][ "loglevel" ].isInt() ) {
        throw( std::exception( __FUNCTION__ ": log_level is missing or invalid" ) );
    }

    return root;
}

uint32_t UcConfig::GetLogLevel()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_logLevel;
}
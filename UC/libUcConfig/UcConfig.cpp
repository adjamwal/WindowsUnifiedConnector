#include "pch.h"
#include "framework.h"
#include "UcConfig.h"
#include "IUcLogger.h"
#include "WindowsUtilities.h"
#include "json\json.h"

#define UC_CONFIG_FILENAME  L"UCService_config.json"

UcConfig::UcConfig() :
    m_logLevel( ( uint32_t )IUcLogger::LOG_ERROR )
{
    if( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, L"Software\\Cisco\\SecureXYZ\\UnifiedConnector\\config", L"Path", m_path ) )
    {
        WLOG_ERROR( L"Failed to read UnifiedConnector config path from registry" );
    }
    else {
        m_path.append( UC_CONFIG_FILENAME );
    }
}

UcConfig::~UcConfig()
{

}

bool UcConfig::LoadConfig()
{
    std::lock_guard<std::mutex> lock( m_mutex );
}

bool UcConfig::VerifyConfig( const std::wstring& path )
{
    const std::wstring& verifyPath = !path.empty() ? path : m_path;

    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
}

uint32_t UcConfig::GetLogLevel()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_logLevel;
}
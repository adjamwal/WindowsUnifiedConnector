#include <Windows.h>
#include "UiVersionStatus.h"
#include "../../GlobalVersion.h"
#include <string>
#include <codecvt>

#define CM_NAME "Cloud Management"
static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> _g_converter;

bool UiVersionStatus::SupportsInterface( const char* pszInterfaceName, unsigned int uiVersion ) 
{
    if( NULL == pszInterfaceName ) {
        return false;
    }

    std::string strInterfaceName = pszInterfaceName;

    // if interface name and version match, return ture
    return ( COMPONENT_STATUS == strInterfaceName && COMPONENT_STATUS_VERSION == uiVersion ) ||
        ( C_COMPONENT_STATUS == strInterfaceName && C_COMPONENT_STATUS_VERSION == uiVersion );
}

PluginResult UiVersionStatus::IsEnabled(bool& rbIsEnabled)
{
    rbIsEnabled = true;
    return Plugin_Success;
}

PluginResult UiVersionStatus::GetDisplayName(char* utf8DisplayString, size_t& nBufferLen)
{
    size_t cm_len = strlen( CM_NAME );

    if( utf8DisplayString == NULL ) {
        nBufferLen = cm_len + 1;
        return Plugin_BufferInvalid;
    }
    
    strncpy_s( utf8DisplayString, nBufferLen, CM_NAME, cm_len );
    return Plugin_Success;
}

PluginResult UiVersionStatus::GetDisplayVersion(char* utfDisplayVersionString, size_t& nBufferLen)
{
    std::string version = _g_converter.to_bytes( STRFORMATPRODVER );
    if( utfDisplayVersionString == NULL ) {
        nBufferLen = version.length() + 1;
        return Plugin_BufferInvalid;
    }

    strncpy_s( utfDisplayVersionString, nBufferLen, version.c_str(), version.length() );
    return Plugin_Success;
}
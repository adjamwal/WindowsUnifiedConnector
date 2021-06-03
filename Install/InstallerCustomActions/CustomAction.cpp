#include "stdafx.h"
#include "PrivateFunctions.h"
#include "MsiLogger.h"
#include <codecvt>

HMODULE globalDllHandle;

UINT __stdcall ExtractCaResources( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;
    std::wstring dllPath;
    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    if ( ExtractResources( dllPath ) ) {
        if ( (hr = MsiSetProperty( hInstall, L"UC_RESOURCE_DIR", dllPath.c_str() ) ) == ERROR_SUCCESS ) {
            WLOG_DEBUG( L"Stored UC_RESOURCE_DIR %s", dllPath.c_str() );
        }
        else {
            WLOG_ERROR( L"MsiSetProperty failed UC_RESOURCE_DIR %s", dllPath.c_str() );
        }
    }

LExit:
    SetUcLogger( NULL );
    return WcaFinalize( ERROR_SUCCESS );
}

UINT __stdcall RemoveCaResources( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;
    MsiLogger msiLogger;
    WCHAR dllPath[ 1024 ] = { 0 };
    DWORD size = 1024;

    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    if ( MsiGetProperty( hInstall, L"UC_RESOURCE_DIR", dllPath, &size ) == ERROR_SUCCESS ) {
        WLOG_DEBUG( L"Remove resource folder %s", dllPath );
        DeleteResources( dllPath );
    }
    else {
        WLOG_ERROR( L"MsiGetProperty failed for UC_RESOURCE_DIR" );
    }

LExit:
    SetUcLogger( NULL );
    return WcaFinalize( ERROR_SUCCESS );
}

UINT __stdcall DetectOlderBuildVersion( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;
    BuildInfo prevBuildInfo;
    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    LPWSTR productName = NULL;
    hr = WcaGetProperty( L"ProductName", &productName );
    ExitOnFailure( hr, "Failed to get ProductName" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ProductName %S", productName );

    if( GetPreviousBuildVersion( prevBuildInfo, productName ) ) {
        WCHAR data[ MAX_PATH ] = { 0 };
        DWORD length = MAX_PATH;

        if( MsiGetPropertyW( hInstall, L"ProductVersion", data, &length ) == 0 ) {
            WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": Target version %S", data );
            BuildInfo currentBuildInfo;
            if( StringToBuildInfo( data, currentBuildInfo ) ) {
                if( currentBuildInfo.versionString == prevBuildInfo.versionString ) {
                    if( prevBuildInfo.buildNumber > currentBuildInfo.buildNumber ) {
                        WcaLog( LOGMSG_STANDARD, "Build Version Downgrade is not permitted" );
                        hr = E_FAIL;
                        goto LExit;
                    }
                }
            }
        }
    }
LExit:
    er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    SetUcLogger( NULL );
    return WcaFinalize( er );
}

UINT __stdcall DetectWindows10OrGreater( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;
    BuildInfo prevBuildInfo;
    const char* filename = "C:\\Windows\\System32\\ntdll.dll";
    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    WcaLog( LOGMSG_STANDARD, "Initialized." );

    hr = IsWindows10OrGreater( GetFileVersion( filename ) ) ? S_OK : E_FAIL;

LExit:
    er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    SetUcLogger( NULL );
    return WcaFinalize( ERROR_SUCCESS );
}

UINT __stdcall CollectUCData( MSIHANDLE hInstall )
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;
    std::string url, ucid, ucidToken;

    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    LPWSTR dllPath = NULL;
    hr = WcaGetProperty( L"UC_RESOURCE_DIR", &dllPath );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_RESOURCE_DIR" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": dllPath %S", dllPath );

    if ( RunCollectUCData( &msiLogger, dllPath, url, ucid, ucidToken ) )
    {
        hr = WcaSetProperty( L"UC_EVENT_URL", converter.from_bytes( url ).c_str() );
        ExitOnFailure( hr, __FUNCTION__ "Failed to set UC_EVENT_URL" );

        hr = WcaSetProperty( L"UC_EVENT_UCID", converter.from_bytes( ucid ).c_str() );
        ExitOnFailure( hr, __FUNCTION__ "Failed to set UC_EVENT_UCID" );

        hr = WcaSetProperty( L"UC_EVENT_UCID_TOKEN", converter.from_bytes( ucidToken ).c_str() );
        ExitOnFailure( hr, __FUNCTION__ "Failed to set UC_EVENT_UCID_TOKEN" );
    }
    else
    {
        WcaLogError( LOGMSG_STANDARD, "Failed to run RunCollectUCData" );
    }

LExit:
    SetUcLogger( NULL );
    //always ignore UCID errors
    return WcaFinalize( ERROR_SUCCESS );
}

UINT __stdcall SendEventOnUninstallBegin( MSIHANDLE hInstall )
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;

    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    LPWSTR dllPath = NULL;
    hr = WcaGetProperty( L"UC_RESOURCE_DIR", &dllPath );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_RESOURCE_DIR" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": dllPath %S", dllPath );

    LPWSTR url = NULL;
    hr = WcaGetProperty( L"UC_EVENT_URL", &url );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_EVENT_URL" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": url %S", url );

    LPWSTR productVersion = NULL;
    hr = WcaGetProperty( L"ProductVersion", &productVersion );
    ExitOnFailure( hr, "Failed to get ProductVersion" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ProductVersion %S", productVersion );

    LPWSTR ucid = NULL;
    hr = WcaGetProperty( L"UC_EVENT_UCID", &ucid );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_EVENT_UCID" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ucid %S", ucid );

    LPWSTR ucidToken = NULL;
    hr = WcaGetProperty( L"UC_EVENT_UCID_TOKEN", &ucidToken );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_EVENT_UCID_TOKEN" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ucidToken %S", ucidToken );

    if ( !RunSendEventOnUninstallBegin( &msiLogger,
        dllPath,
        converter.to_bytes( url ),
        converter.to_bytes( productVersion ),
        converter.to_bytes( ucid ),
        converter.to_bytes( ucidToken ) ) )
    {
        WcaLogError( LOGMSG_STANDARD, "Failed to run RunSendEventOnUninstallBegin" );
    }

LExit:
    SetUcLogger( NULL );
    return WcaFinalize( ERROR_SUCCESS );
}

UINT __stdcall SendEventOnUninstallError( MSIHANDLE hInstall )
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;

    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    LPWSTR dllPath = NULL;
    hr = WcaGetProperty( L"UC_RESOURCE_DIR", &dllPath );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_RESOURCE_DIR" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": dllPath %S", dllPath );

    LPWSTR url = NULL;
    hr = WcaGetProperty( L"UC_EVENT_URL", &url );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_EVENT_URL" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": url %S", url );

    LPWSTR productVersion = NULL;
    hr = WcaGetProperty( L"ProductVersion", &productVersion );
    ExitOnFailure( hr, "Failed to get ProductVersion" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ProductVersion %S", productVersion );

    LPWSTR ucid = NULL;
    hr = WcaGetProperty( L"UC_EVENT_UCID", &ucid );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_EVENT_UCID" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ucid %S", ucid );

    LPWSTR ucidToken = NULL;
    hr = WcaGetProperty( L"UC_EVENT_UCID_TOKEN", &ucidToken );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_EVENT_UCID_TOKEN" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ucidToken %S", ucidToken );

    if ( !RunSendEventOnUninstallError( &msiLogger,
        dllPath,
        converter.to_bytes( url ),
        converter.to_bytes( productVersion ),
        converter.to_bytes( ucid ),
        converter.to_bytes( ucidToken ) ) )
    {
        WcaLogError( LOGMSG_STANDARD, "Failed to run RunSendEventOnUninstallError" );
    }

LExit:
    SetUcLogger( NULL );
    return WcaFinalize( ERROR_SUCCESS );
}

UINT __stdcall SendEventOnUninstallComplete( MSIHANDLE hInstall )
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;

    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    LPWSTR dllPath = NULL;
    hr = WcaGetProperty( L"UC_RESOURCE_DIR", &dllPath );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_RESOURCE_DIR" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": dllPath %S", dllPath );

    LPWSTR url = NULL;
    hr = WcaGetProperty( L"UC_EVENT_URL", &url );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_EVENT_URL" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": url %S", url );

    LPWSTR productVersion = NULL;
    hr = WcaGetProperty( L"ProductVersion", &productVersion );
    ExitOnFailure( hr, "Failed to get ProductVersion" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ProductVersion %S", productVersion );

    LPWSTR ucid = NULL;
    hr = WcaGetProperty( L"UC_EVENT_UCID", &ucid );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_EVENT_UCID" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ucid %S", ucid );

    LPWSTR ucidToken = NULL;
    hr = WcaGetProperty( L"UC_EVENT_UCID_TOKEN", &ucidToken );
    ExitOnFailure( hr, __FUNCTION__ "Failed to get UC_EVENT_UCID_TOKEN" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ucidToken %S", ucidToken );

    if ( !RunSendEventOnUninstallComplete( &msiLogger,
        dllPath,
        converter.to_bytes( url ),
        converter.to_bytes( productVersion ),
        converter.to_bytes( ucid ),
        converter.to_bytes( ucidToken ) ) )
    {
        WcaLogError( LOGMSG_STANDARD, "Failed to run RunSendEventOnUninstallComplete" );
    }

LExit:
    SetUcLogger( NULL );
    return WcaFinalize( ERROR_SUCCESS );
}

// DllMain - Initialize and cleanup WiX custom action utils.
extern "C" BOOL WINAPI DllMain(
    __in HINSTANCE hInst,
    __in ULONG ulReason,
    __in LPVOID
)
{
    globalDllHandle = hInst;
    switch( ulReason )
    {
    case DLL_PROCESS_ATTACH:
        WcaGlobalInitialize( hInst );
        break;

    case DLL_PROCESS_DETACH:
        WcaGlobalFinalize();
        break;
    }

    return TRUE;
}


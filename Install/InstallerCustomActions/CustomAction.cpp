#include "stdafx.h"
#include "PrivateFunctions.h"
#include "MsiLogger.h"
#include <codecvt>
#include <filesystem>

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

    TryKillProcessByName( L"csc_cmdt.exe" );

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

UINT __stdcall DetectValidWindowsVersion( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;
    const char* filename = "C:\\Windows\\System32\\ntdll.dll";
    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    WcaLog( LOGMSG_STANDARD, "Initialized." );

    if ( IsArmCpu() ) {
        hr = IsWindows11OrGreater( GetFileVersion( filename ) ) ? S_OK : E_FAIL;
    }
    else {
        hr = IsWindows10OrGreater(GetFileVersion( filename ) ) ? S_OK : E_FAIL;
    }


LExit:
    er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    SetUcLogger( NULL );
    return WcaFinalize( er );
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

UINT __stdcall SetupRemoveCscPluginData( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    LPWSTR tempPath = NULL;
    std::wstring customData;

    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    BuildCustomActionData( { L"CSC_PLUGIN_DIRECTORY", L"UC_INSTALLCACHE_DIR" }, '|', customData );

    if( !customData.length() ) {
        hr = E_FAIL;
        ExitOnFailure( hr, __FUNCTION__ "Invalid CustomActionData" );
    }

    if( ( hr = MsiSetProperty( hInstall, L"REMOVE_CSC_PLUGIN_DATA", customData.c_str() ) ) == ERROR_SUCCESS ) {
        WLOG_DEBUG( L"Stored REMOVE_CSC_PLUGIN_DATA %s", customData.c_str() );
    }
    else {
        WLOG_ERROR( L"MsiSetProperty failed REMOVE_CSC_PLUGIN_DATA %s", customData.c_str() );
    }
LExit:
    SetUcLogger( NULL );
    return WcaFinalize( hr );
}

UINT __stdcall RemoveCscPlugin( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    LPWSTR tempPath = NULL;
    WCHAR customActionData[ 2048 ] = { 0 };
    DWORD customActionDataSize = 2048;
    std::filesystem::path pluginPath;
    std::filesystem::path cachePath;

    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    if( MsiGetProperty( hInstall, L"CustomActionData", customActionData, &customActionDataSize ) == ERROR_SUCCESS ) {
        WLOG_DEBUG( L"CustomActionData %s", customActionData );
        std::wstring tempStr = customActionData;
        std::vector<std::wstring> customActionDataList;
        Tokenize( customActionData, '|', customActionDataList );
        if( customActionDataList.size() >= 2 ) {
            pluginPath = customActionDataList[ 0 ];
            pluginPath /= "csccmplugin.dll";

            cachePath = customActionDataList[ 1 ];
        }
        else {
            ExitOnFailure( hr, __FUNCTION__ "Invalid CustomActionData" );
        }
    }
    else {
        ExitOnFailure( hr, __FUNCTION__ "Failed to get CustomActionData" );
    }

    if( !MovePluginForDeletion( pluginPath, cachePath ) ) {
        WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": Failed to delete pluging %S", pluginPath.c_str() );
    }

LExit:
    SetUcLogger( NULL );
    return WcaFinalize( ERROR_SUCCESS );
}

UINT __stdcall SetupCopyCscPluginData( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    LPWSTR tempPath = NULL;
    std::wstring customData;

    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );

    BuildCustomActionData( { L"CSC_PLUGIN_DIRECTORY", L"UCSERVICE_DIRECTORY", L"UC_INSTALLCACHE_DIR" }, '|', customData );

    if( !customData.length() ) {
        hr = E_FAIL;
        ExitOnFailure( hr, __FUNCTION__ "Invalid CustomActionData" );
    }
    
    if( ( hr = MsiSetProperty( hInstall, L"COPY_CSC_PLUGIN_DATA", customData.c_str() ) ) == ERROR_SUCCESS ) {
        WLOG_DEBUG( L"Stored COPY_CSC_PLUGIN_DATA %s", customData.c_str() );
    }
    else {
        WLOG_ERROR( L"MsiSetProperty failed COPY_CSC_PLUGIN_DATA %s", customData.c_str() );
    }
LExit:
    SetUcLogger( NULL );
    return WcaFinalize( hr );
}

UINT __stdcall CopyCscPlugin( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    LPWSTR tempPath = NULL;
    WCHAR customActionData[ 2048 ] = { 0 };
    DWORD customActionDataSize = 2048;
    std::filesystem::path pluginPath;
    std::filesystem::path ucPlugingPath;
    std::filesystem::path tmpPlugingPath;
    std::filesystem::path cachePath;
    std::error_code ec;

    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, __FUNCTION__ "Failed to initialize" );


    if( MsiGetProperty( hInstall, L"CustomActionData", customActionData, &customActionDataSize ) == ERROR_SUCCESS ) {
        WLOG_DEBUG( L"CustomActionData %s", customActionData );
        std::wstring tempStr = customActionData;
        std::vector<std::wstring> customActionDataList;
        Tokenize( customActionData, '|', customActionDataList );
        if( customActionDataList.size() >= 3 ) {
            pluginPath = customActionDataList[ 0 ];
            pluginPath /= "csccmplugin.dll";

            ucPlugingPath = customActionDataList[ 1 ];
            ucPlugingPath /= "csccmplugin.dll";

            tmpPlugingPath = ucPlugingPath;
            tmpPlugingPath += ".tmp";

            cachePath = customActionDataList[ 2 ];
        }
        else {
            ExitOnFailure( hr, __FUNCTION__ "Invalid CustomActionData" );
        }
    }
    else {
        ExitOnFailure( hr, __FUNCTION__ "Failed to get CustomActionData" );
    }

    if( std::filesystem::exists( pluginPath ) ) {
        if( !MovePluginForDeletion( pluginPath, cachePath ) ) {
            hr = E_FAIL;
            ExitOnFailure( hr, __FUNCTION__ "Failed to get move existing plugin" );
        }
    }

    std::filesystem::copy_file( ucPlugingPath, tmpPlugingPath, std::filesystem::copy_options::overwrite_existing, ec);
    if( ec ) {
        WcaLogError( LOGMSG_STANDARD, __FUNCTION__ ": Failed to copy %S to %S. %s", ucPlugingPath.c_str(), tmpPlugingPath.c_str(), ec.message().c_str() );
        hr = E_FAIL;
    }

    std::filesystem::create_directories( pluginPath.parent_path() );
    std::filesystem::rename( tmpPlugingPath, pluginPath, ec );

    if( ec ) {
        WcaLogError( LOGMSG_STANDARD, __FUNCTION__ ": Failed to move %S to %S. %s", tmpPlugingPath.c_str(), pluginPath.c_str(), ec.message().c_str() );
        hr = E_FAIL;
    }

LExit:
    SetUcLogger( NULL );
    return WcaFinalize( hr );
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


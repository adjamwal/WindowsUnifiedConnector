#include "stdafx.h"
#include "PrivateFunctions.h"
#include "MsiLogger.h"

UINT __stdcall DetectOlderBuildVersion( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;
    BuildInfo prevBuildInfo;
    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, "Failed to initialize" );

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
    ExitOnFailure( hr, "Failed to initialize" );

    WcaLog( LOGMSG_STANDARD, "Initialized." );

    hr = IsWindows10OrGreater( GetFileVersion( filename ) ) ? S_OK : E_FAIL;

LExit:
    er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    SetUcLogger( NULL );
    return WcaFinalize( er );
}

UINT __stdcall StoreUCIDToProperty( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;
    std::string ucidStr;
    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    WcaLog( LOGMSG_STANDARD, __FUNCTION__ );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, "Failed to initialize" );

    ucidStr = GetNewUCIDToken();
    if( hr = MsiSetPropertyA( hInstall, "UC_EVENT_UCID", ucidStr.c_str() ) == ERROR_SUCCESS )
    {
        WcaLog( LOGMSG_STANDARD, "Stored UCID: %s", ucidStr.c_str() );
    }
    else
    {
        WcaLogError( LOGMSG_STANDARD, "Failed to store UCID value: %s", ucidStr.c_str() );
    }

LExit:
    er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    SetUcLogger( NULL );
    return WcaFinalize( er );
}

UINT __stdcall SendEventOnUninstallBegin( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;
    char ucidToken[ MAX_PATH ] = { 0 };
    DWORD ucidTokenLength = 0;
    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    WcaLog( LOGMSG_STANDARD, __FUNCTION__ );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, "Failed to initialize" );

    LPWSTR productVersion = NULL;
    hr = WcaGetProperty( L"ProductVersion", &productVersion );
    ExitOnFailure( hr, "Failed to get ProductVersion" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ProductVersion %s", productVersion );

    if( hr = MsiGetPropertyA( hInstall, "UC_EVENT_UCID", ucidToken, &ucidTokenLength ) == ERROR_SUCCESS )
    {
        WcaLog( LOGMSG_STANDARD, "Loaded UCID: %s", ucidToken );
    }
    else
    {
        WcaLogError( LOGMSG_STANDARD, "Failed to load stored UCID value" );
    }

    if( ucidTokenLength <= 1 || !NotifyUninstallBeginEvent( ucidToken, productVersion ) )
    {
        WcaLogError( LOGMSG_STANDARD, "Error sending uninstall BEGIN event" );
    }
    else
    {
        WcaLog( LOGMSG_STANDARD, "Successfully sent uninstall BEGIN event" );
    }

LExit:
    er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    SetUcLogger( NULL );
    return WcaFinalize( er );
}

/*
NOTES on sending event on uninstall error and uninstall finalization:
https://stackoverflow.com/questions/45918457/wix-customaction-based-on-the-outcome-of-another-customaction
1. for error, send event from a Rollback Custom Action (Execute="rollback") scheduled before InstallFinalize,
   with condition (REMOVE~="ALL") AND (NOT UPGRADINGPRODUCTCODE)
   http://www.installsite.org/pages/en/isnews/200108/

2. on uninstall end, we can send an event from a normal custom action scheduled After="InstallFinalize",
   with condition (REMOVE~="ALL") AND (NOT UPGRADINGPRODUCTCODE)
   this CA won't be aware if there were errors prior to this step,
   as the "rollback"  and deferred CAs get executed by a different set of generated scripts
   which cannot set msi properties to be consumed by the normal execution CAs

other docs:
https://stackoverflow.com/questions/11518927/custom-action-after-installation-failed
https://docs.microsoft.com/en-us/windows/win32/api/msiquery/nf-msiquery-msigetmode
to find out if we are in rollback mode: MsiGetMode MSIRUNMODE_ROLLBACK ==> true??

to find out if a product is still installed:
MsiGetFeatureStateA ==> INSTALLSTATE_ABSENT https://docs.microsoft.com/en-us/windows/win32/api/msiquery/nf-msiquery-msigetfeaturestatea
MsiGetFeatureValidStatesA ==> INSTALLSTATE_ABSENT https://docs.microsoft.com/en-us/windows/win32/api/msiquery/nf-msiquery-msigetfeaturevalidstatesa
MsiOpenProductA ==> ERROR_UNKNOWN_PRODUCT https://docs.microsoft.com/en-us/windows/win32/api/msi/nf-msi-msiopenproducta?redirectedfrom=MSDN
*/

//dummy deferred CA - rollback CA needs pairing with a deferred custom action to rollback on uninstall failure
//http://www.installsite.org/pages/en/isnews/200108/
UINT __stdcall NoopDeferredUninstallAction( MSIHANDLE hInstall )
{
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ );
    return WcaFinalize( ERROR_SUCCESS );
}

//NOTE this CA cannot modify or store data in msi properties
//https://stackoverflow.com/questions/48459445/how-to-set-wix-property-from-deferred-customaction-c-dll
//NOTE: this CA won't run if installer has RollbackDisabled set
//https://docs.microsoft.com/en-us/windows/win32/msi/commit-custom-actions
//NOTE: a rollback action must have a corresponding deferred action to rollback (??)
//http://www.installsite.org/pages/en/isnews/200108/
UINT __stdcall SendEventOnUninstallRollback( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;

    char ucidToken[ MAX_PATH ] = { 0 };
    DWORD ucidTokenLength = 0;

    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    WcaLog( LOGMSG_STANDARD, __FUNCTION__ );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, "Failed to initialize" );

    if( MsiGetMode( hInstall, MSIRUNMODE_ROLLBACK ) == TRUE )
    {
        WcaLog( LOGMSG_STANDARD, "ROLLBACK MODE DETECTED" );

        LPWSTR productVersion = NULL;
        hr = WcaGetProperty( L"ProductVersion", &productVersion );
        ExitOnFailure( hr, "Failed to get ProductVersion" );
        WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ProductVersion %S", productVersion );

        if( hr = MsiGetPropertyA( hInstall, "UC_EVENT_UCID", ucidToken, &ucidTokenLength ) == ERROR_SUCCESS )
        {
            WcaLog( LOGMSG_STANDARD, "Loaded UCID: %s", ucidToken );
        }
        else
        {
            WcaLogError( LOGMSG_STANDARD, "Failed to load stored UCID value" );
        }

        if( ucidTokenLength <= 1 || !NotifyUninstallFailureEvent( ucidToken, productVersion ) )
        {
            WcaLogError( LOGMSG_STANDARD, "Error sending uninstall FAILURE event" );
        }
        else
        {
            WcaLog( LOGMSG_STANDARD, "Successfully sent uninstall FAILURE event" );
        }
    }

LExit:
    er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    SetUcLogger( NULL );
    return WcaFinalize( er );
}

//NOTE: this CA cannot know whether a rollback had occurred before completion
//since custom properties cannot be set during rollbacks
UINT __stdcall SendEventOnUninstallComplete( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;

    char ucidToken[ MAX_PATH ] = { 0 };
    DWORD ucidTokenLength = 0;

    char rollback[ MAX_PATH ] = { 0 };
    DWORD rollbackLength = 0;
    bool hasRollbackBeenDetected = false;

    MsiLogger msiLogger;
    SetUcLogger( &msiLogger );

    WcaLog( LOGMSG_STANDARD, __FUNCTION__ );

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, "Failed to initialize" );

    LPWSTR productVersion = NULL;
    hr = WcaGetProperty( L"ProductVersion", &productVersion );
    ExitOnFailure( hr, "Failed to get ProductVersion" );
    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": ProductVersion %S", productVersion );

    if( hr = MsiGetPropertyA( hInstall, "UC_EVENT_UCID", ucidToken, &ucidTokenLength ) == ERROR_SUCCESS )
    {
        WcaLog( LOGMSG_STANDARD, "Loaded UCID: %s", ucidToken );
    }
    else
    {
        WcaLogError( LOGMSG_STANDARD, "Failed to load stored UCID value" );
    }

    if( ucidTokenLength <= 1 || !NotifyUninstallEndEvent( ucidToken, productVersion ) )
    {
        WcaLogError( LOGMSG_STANDARD, "Error sending uninstall END event" );
    }
    else
    {
        WcaLog( LOGMSG_STANDARD, "Successfully sent uninstall END event");
    }

LExit:
    er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    SetUcLogger( NULL );
    return WcaFinalize( er );
}

// DllMain - Initialize and cleanup WiX custom action utils.
extern "C" BOOL WINAPI DllMain(
    __in HINSTANCE hInst,
    __in ULONG ulReason,
    __in LPVOID
)
{
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

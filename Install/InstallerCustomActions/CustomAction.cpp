#include "stdafx.h"
#include "PrivateFunctions.h"

UINT __stdcall DetectOlderBuildVersion( MSIHANDLE hInstall )
{
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;
    BuildInfo prevBuildInfo;

	hr = WcaInitialize(hInstall, __FUNCTION__ );
	ExitOnFailure(hr, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

    LPWSTR productName = NULL;
    hr = WcaGetProperty( L"ProductName", &productName );
    ExitOnFailure( hr, "Failured to get ProductName" );
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
	er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

UINT __stdcall DetectWindows10OrGreater( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;
    BuildInfo prevBuildInfo;
    const char* filename = "C:\\Windows\\System32\\ntdll.dll";

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, "Failed to initialize" );

    WcaLog( LOGMSG_STANDARD, "Initialized." );

    hr = IsWindows10OrGreater( GetFileVersion( filename ) ) ? S_OK : E_FAIL;

LExit:
    er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    return WcaFinalize( er );
}

UINT __stdcall StoreUCIDToProperty( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, "Failed to initialize" );

    std::string evUcidPropVal = GetNewUCID();
    auto result = MsiSetPropertyA( hInstall, "UC_EVENT_UCID", evUcidPropVal.c_str() );
    WcaLog( LOGMSG_STANDARD, "Stored UCID: %s", evUcidPropVal.c_str() );

LExit:
    er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    return WcaFinalize( er );
}

UINT __stdcall SendUninstallEvent( MSIHANDLE hInstall )
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;

    hr = WcaInitialize( hInstall, __FUNCTION__ );
    ExitOnFailure( hr, "Failed to initialize" );

    char evUcidPropVal[ MAX_PATH ] = { 0 };
    DWORD evUcidPropValLength = 0;
    if( MsiGetPropertyA( hInstall, "UC_EVENT_UCID", evUcidPropVal, &evUcidPropValLength ) == 0 )
    {
        WcaLog( LOGMSG_STANDARD, "Loaded UCID: %s", evUcidPropVal );
        //send event
    }

LExit:
    er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
    return WcaFinalize( er );
}

// DllMain - Initialize and cleanup WiX custom action utils.
extern "C" BOOL WINAPI DllMain(
	__in HINSTANCE hInst,
	__in ULONG ulReason,
	__in LPVOID
	)
{
	switch(ulReason)
	{
	case DLL_PROCESS_ATTACH:
		WcaGlobalInitialize(hInst);
		break;

	case DLL_PROCESS_DETACH:
		WcaGlobalFinalize();
		break;
	}

	return TRUE;
}

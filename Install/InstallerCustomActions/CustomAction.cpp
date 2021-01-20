#include "stdafx.h"
#include <Msi.h>
#include <string>
#include <versionhelpers.h>

#define GUID_SIZE 39

struct BuildInfo
{
    std::wstring versionString;
    int buildNumber;
};

bool StringToBuildInfo( const std::wstring& version, BuildInfo& buildInfo )
{
    buildInfo.versionString = version;
    size_t idx = buildInfo.versionString.find_last_of( '.' );

    if( idx != std::wstring::npos ) {
        std::wstring strProductVersion = buildInfo.versionString.substr( idx + 1 );
        buildInfo.versionString = buildInfo.versionString.substr( 0, idx );
        buildInfo.buildNumber = _wtoi( strProductVersion.c_str() );
        WcaLog( LOGMSG_STANDARD, __FUNCTION__ " Version %S BuildNumber %d\n", buildInfo.versionString.c_str(), buildInfo.buildNumber );
        
        return true;
    }
    else {
        buildInfo.versionString.clear();
        WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": Invalid Version String %S", version.c_str() );
    }

    return false;
}

bool GetPreviousBuildVersion( BuildInfo& buildInfo, LPWSTR& productName )
{
    DWORD dwIndex = 0;
    DWORD dwStatus = ERROR_SUCCESS;
    DWORD cchdata = MAX_PATH;
    WCHAR data[ MAX_PATH ] = { 0 };
    wchar_t szProductCode[ GUID_SIZE ] = { 0 };

    do {
        memset( szProductCode, 0, sizeof( szProductCode ) );
        dwStatus = MsiEnumProductsW( dwIndex, szProductCode );
        if( dwStatus != ERROR_SUCCESS ) {
            WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": could not enumerate products. %d", dwStatus );
            break;
        }

        cchdata = MAX_PATH;
        if( MsiGetProductInfoW( szProductCode, INSTALLPROPERTY_PRODUCTNAME, data, &cchdata ) == 0 ) {
            if( wcscmp( data, productName ) == 0 ) {
                cchdata = MAX_PATH;
                if( ( dwStatus = MsiGetProductInfoW( szProductCode, INSTALLPROPERTY_VERSIONSTRING, data, &cchdata ) ) == 0 ) {
                    WcaLog( LOGMSG_STANDARD, __FUNCTION__ ": found installed version %S", data );
                    return StringToBuildInfo( data, buildInfo );
                }
            }
        }
        dwIndex++;
    } while( dwStatus == ERROR_SUCCESS );

    return false;
}

UINT __stdcall DetectOlderBuildVersion(
	MSIHANDLE hInstall
	)
{
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;
    BuildInfo prevBuildInfo;

	hr = WcaInitialize(hInstall, "DetectOlderBuildVersion");
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


std::string GetFileVersion( const std::string filename )
{
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeA( filename.c_str(), &handle );
    void* block = NULL;
    std::string fileVersion;

    if ( size ) {
        block = calloc( 1, size );
        if ( block ) {
            if ( GetFileVersionInfoA( filename.c_str(), 0, size, block ) ) {
                HRESULT hr;
                UINT cbTranslate = 0;

                struct LANGANDCODEPAGE
                {
                    WORD wLanguage;
                    WORD wCodePage;
                } *lpTranslate;

                // Read the list of languages and code pages.

                VerQueryValueA( block,
                    "\\VarFileInfo\\Translation",
                    ( LPVOID* )&lpTranslate,
                    &cbTranslate );

                // Read the file description for each language and code page.

                for ( int i = 0; i < ( cbTranslate / sizeof( struct LANGANDCODEPAGE ) ); i++ ) {
                    char SubBlock[ 100 ] = { 0 };
                    hr = StringCchPrintfA( SubBlock, 50,
                        "\\StringFileInfo\\%04x%04x\\FileVersion",
                        lpTranslate[ i ].wLanguage,
                        lpTranslate[ i ].wCodePage );
                    if ( FAILED( hr ) ) {
                        // TODO: write error handler.
                    }

                    PVOID lpBuffer = NULL;
                    UINT dwBytes = 0;
                    // Retrieve file description for language and code page "i". 
                    VerQueryValueA( block,
                        SubBlock,
                        &lpBuffer,
                        &dwBytes );

                    fileVersion.assign( ( char* )lpBuffer, dwBytes );
                    WcaLog( LOGMSG_STANDARD, "File Version %s", fileVersion.c_str() );
                    break;
                }
            }
            else {
                WcaLog( LOGMSG_STANDARD, "GetFileVersionInfo failed." );
            }
        }
        else {
            WcaLog( LOGMSG_STANDARD, "calloc failed." );
        }
    }
    else {
        WcaLog( LOGMSG_STANDARD, "GetFileVersionInfoSizeA failed." );
    }


    if ( block ) {
        free( block );
    }

    return fileVersion;
}

bool IsGreaterThanWindows10( const std::string fileVersion )
{
    bool rtn = false;
    std::string majorVersion;
    size_t dot = fileVersion.find_first_of( "." );
    if ( dot != std::string::npos ) {
        majorVersion = fileVersion.substr( 0, dot );
    }

    if ( !majorVersion.empty() && atoi( majorVersion.c_str() ) >= 10 ) {
        rtn = true;
    }

    return rtn;
}

UINT __stdcall DetectWindows10OrGreater(
    MSIHANDLE hInstall
)
{
    HRESULT hr = S_OK;
    UINT er = ERROR_SUCCESS;
    BuildInfo prevBuildInfo;
    const char* filename = "C:\\Windows\\System32\\ntdll.dll";

    hr = WcaInitialize( hInstall, "DetectWindows10OrGreater" );
    ExitOnFailure( hr, "Failed to initialize" );

    WcaLog( LOGMSG_STANDARD, "Initialized." );

    hr = IsGreaterThanWindows10( GetFileVersion( filename ) ) ? S_OK : E_FAIL;

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

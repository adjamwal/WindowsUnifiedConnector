#include "stdafx.h"
#include "PrivateFunctions.h"
#include "CloudEventBuilder.h"
#include "CloudEventPublisher.h"
#include "CloudEventStorage.h"
#include "PmHttp.h"
#include "PmCloud.h"
#include "PmConfig.h"
#include "PmConstants.h"
#include "FileSysUtil.h"
#include "StringUtil.h"

bool PrepareAndSendEvent( std::string ucidToken, std::wstring productVersion, int errorCode, std::string errorMessage )
{
    CloudEventBuilder ev;
    FileSysUtil fileUtil;
    PmHttp pmHttp(fileUtil);
    PmCloud pmCloud( pmHttp );
    CloudEventStorage eventStorage( CLOUD_EVENT_STORAGE_FILENAME, fileUtil);
    PmConfig pmConfig(fileUtil);
    CloudEventPublisher epub(pmCloud, eventStorage, pmConfig);

    ev.WithUCID( ucidToken );
    ev.WithPackageID( "uc/" + StringUtil::WStr2Str(productVersion) );
    ev.WithType( CloudEventType::pkguninstall );
    ev.WithError( errorCode, errorMessage );

    epub.SetToken( ucidToken );
    return epub.Publish( ev ) == 200;
}

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

std::string GetFileVersion( const std::string filename )
{
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeA( filename.c_str(), &handle );
    void* block = NULL;
    std::string fileVersion;

    if( size ) {
        block = calloc( 1, size );
        if( block ) {
            if( GetFileVersionInfoA( filename.c_str(), 0, size, block ) ) {
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

                for( int i = 0; i < ( cbTranslate / sizeof( struct LANGANDCODEPAGE ) ); i++ ) {
                    char SubBlock[ 100 ] = { 0 };
                    hr = StringCchPrintfA( SubBlock, 50,
                        "\\StringFileInfo\\%04x%04x\\FileVersion",
                        lpTranslate[ i ].wLanguage,
                        lpTranslate[ i ].wCodePage );
                    if( FAILED( hr ) ) {
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

    if( block ) {
        free( block );
    }

    return fileVersion;
}

bool IsWindows10OrGreater( const std::string fileVersion )
{
    bool rtn = false;
    std::string majorVersion;
    size_t dot = fileVersion.find_first_of( "." );
    if( dot != std::string::npos ) {
        majorVersion = fileVersion.substr( 0, dot );
    }

    if( !majorVersion.empty() && atoi( majorVersion.c_str() ) >= 10 ) {
        rtn = true;
    }

    return rtn;
}

std::string GetNewUCIDToken()
{
    return "test-ucid";
}

bool NotifyUninstallBeginEvent( std::string ucidToken, std::wstring productVersion )
{
    return PrepareAndSendEvent( 
        ucidToken, productVersion, 
        UCPM_EVENT_UC_UNINSTALL_START, "UC uninstall started" );
}

bool NotifyUninstallFailureEvent( std::string ucidToken, std::wstring productVersion )
{
    return PrepareAndSendEvent( 
        ucidToken, productVersion, 
        UCPM_EVENT_UC_UNINSTALL_ROLLBACK, "UC uninstall has encountered an error and is being rolled back" );
}

bool NotifyUninstallEndEvent( std::string ucidToken, std::wstring productVersion )
{
    return PrepareAndSendEvent( 
        ucidToken, productVersion, 
        UCPM_EVENT_UC_UNINSTALL_END, "UC uninstall ended" );
}

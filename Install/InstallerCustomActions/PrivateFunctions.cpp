#include "stdafx.h"
#include "PrivateFunctions.h"
#include <Rpc.h>
#include "IUcLogger.h"
#include <filesystem>
#include <exception>
#include "resource.h"
#include <tchar.h>
#include <codecvt>
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>

extern HMODULE globalDllHandle;

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

                for( size_t i = 0; i < ( cbTranslate / sizeof( struct LANGANDCODEPAGE ) ); i++ ) {
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

bool IsArmCpu()
{
    SYSTEM_INFO SystemInfo;
    GetNativeSystemInfo( &SystemInfo );

    bool rtn = ( SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM ) ||
               ( SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64 );

    WLOG_DEBUG( L"IsArmCpu: %s", rtn ? L"true" : L"false" );

    return rtn;
}

bool ExtractResourceToFile( HMODULE dllHandle, LPCTSTR ResourceName, LPCTSTR ResourceType, LPCTSTR OutputFileName )
{
    WLOG_DEBUG( L"Extracting Resource %s", OutputFileName );

    HRSRC hResource = ::FindResource( dllHandle, ResourceName, ResourceType );
    if( hResource == NULL )
        return false;

    HGLOBAL hLoadResource = ::LoadResource( dllHandle, hResource );
    if( hLoadResource == NULL )
        return false;

    LPVOID pResource = ::LockResource( hLoadResource );
    if( pResource == NULL )
        return false;

    DWORD dwResSize = ::SizeofResource( dllHandle, hResource );
    if( dwResSize == 0 )
        return false;

    HANDLE hFile = ::CreateFile( OutputFileName, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    if( hFile == INVALID_HANDLE_VALUE )
        return false;

    DWORD dwBytesWritten = 0;
    if( !::WriteFile( hFile, pResource, dwResSize, &dwBytesWritten, NULL ) ) {
        ::CloseHandle( hFile );
        ::DeleteFile( OutputFileName );

        return false;
    }

    ::CloseHandle( hFile );

    return true;
}

std::string GenerateNewGuid()
{
    RPC_CSTR rpcStr = NULL;
    UUID guid = { 0 };
    std::string guidRtn;

    if( UuidCreate( &guid ) != RPC_S_OK ) {
        LOG_ERROR( "UuidCreate failed" );
        goto abort;
    }

    if( UuidToStringA( &guid, &rpcStr ) != RPC_S_OK ) {
        LOG_ERROR( "UuidToString failed" );
        goto abort;
    }

    guidRtn = ( char* )rpcStr;
    RpcStringFreeA( &rpcStr );

abort:
    return guidRtn;
}

bool ExtractResources( std::wstring& outputPath )
{
    std::filesystem::path tempPath = std::filesystem::temp_directory_path();
    std::filesystem::path rcPath;
    bool retErr = true;
    tempPath /= "UC.Installer-";
    tempPath += GenerateNewGuid();

    std::filesystem::create_directories( tempPath );

    rcPath = tempPath / MSVCP_DLL_NAME;
    if( !ExtractResourceToFile( globalDllHandle, MAKEINTRESOURCE( IDR_RRDLL_MSVCP ), RT_RCDATA, rcPath.native().c_str() ) ) {
        WLOG_ERROR( L"Failed to extract %s", rcPath.native().c_str() );
        retErr = false;
    }

    rcPath = tempPath / MSVCP1_DLL_NAME;
    if( !ExtractResourceToFile( globalDllHandle, MAKEINTRESOURCE( IDR_RRDLL_MSVCP1 ), RT_RCDATA, rcPath.native().c_str() ) ) {
        WLOG_ERROR( L"Failed to extract %s", rcPath.native().c_str() );
        retErr = false;
    }

    rcPath = tempPath / MSVCP2_DLL_NAME;
    if( !ExtractResourceToFile( globalDllHandle, MAKEINTRESOURCE( IDR_RRDLL_MSVCP2 ), RT_RCDATA, rcPath.native().c_str() ) ) {
        WLOG_ERROR( L"Failed to extract %s", rcPath.native().c_str() );
        retErr = false;
    }

    rcPath = tempPath / VCRUNTIME_DLL_NAME;
    if( !ExtractResourceToFile( globalDllHandle, MAKEINTRESOURCE( IDR_RRDLL_VCRUNTIME ), RT_RCDATA, rcPath.native().c_str() ) ) {
        WLOG_ERROR( L"Failed to extract %s", rcPath.native().c_str() );
        retErr = false;
    }

    rcPath = tempPath / UCRT_DLL_NAME;
    if( !ExtractResourceToFile( globalDllHandle, MAKEINTRESOURCE( IDR_RRDLL_UCRT ), RT_RCDATA, rcPath.native().c_str() ) ) {
        WLOG_ERROR( L"Failed to extract %s", rcPath.native().c_str() );
        retErr = false;
    }

    rcPath = tempPath / CASUPPORT_DLL_NAME;
    if( !ExtractResourceToFile( globalDllHandle, MAKEINTRESOURCE( IDR_RRDLL_CASUPPORT ), RT_RCDATA, rcPath.native().c_str() ) ) {
        WLOG_ERROR( L"Failed to extract %s", rcPath.native().c_str() );
        retErr = false;
    }

    rcPath = tempPath / CRYPTO_DLL_NAME;
    if( !ExtractResourceToFile( globalDllHandle, MAKEINTRESOURCE( IDR_RRDLL_CRYPTO ), RT_RCDATA, rcPath.native().c_str() ) ) {
        WLOG_ERROR( L"Failed to extract %s", rcPath.native().c_str() );
        retErr = false;
    }

    rcPath = tempPath / SSL_DLL_NAME;
    if( !ExtractResourceToFile( globalDllHandle, MAKEINTRESOURCE( IDR_RRDLL_SSL ), RT_RCDATA, rcPath.native().c_str() ) ) {
        WLOG_ERROR( L"Failed to extract %s", rcPath.native().c_str() );
        retErr = false;
    }

#ifdef _DEBUG
    rcPath = tempPath / ZLIB_DLL_NAME;
    if( !ExtractResourceToFile( globalDllHandle, MAKEINTRESOURCE( IDR_RRDLL_ZLIB ), RT_RCDATA, rcPath.native().c_str() ) ) {
        WLOG_ERROR( L"Failed to extract %s", rcPath.native().c_str() );
        retErr = false;
    }
#endif

    if( retErr ) {
        outputPath = tempPath.native();
    }

    return retErr;
}

bool DeleteResources( const std::wstring& dllPath )
{
    bool rtn = true;

    if( dllPath.empty() || !std::filesystem::exists( dllPath ) ) return rtn;

    try {
        std::filesystem::remove_all( dllPath );
    }
    catch( ... ) { }

    if( !std::filesystem::exists( dllPath ) ) return rtn;

    try {
        for( auto& entry : std::filesystem::directory_iterator( dllPath ) )
        {
            if( entry.is_regular_file() )
            {
                ::MoveFileEx( entry.path().c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT );
            }
        }
        //https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexw
        ::MoveFileEx( dllPath.c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT );

        WLOG_DEBUG( L"Deferred removal of %s", dllPath.c_str() );
    }
    catch( std::exception& e ) {
        WLOG_ERROR( L"Failed to defer removal of %s: %hs", dllPath.c_str(), e.what() );
        rtn = false;
    }

    return rtn;
}

HMODULE LoadCaSupportDll( const std::wstring& dllPath )
{
    HMODULE module = NULL;

    WLOG_DEBUG( L"Loading dll %s\\%hs", dllPath.c_str(), CASUPPORT_DLL_NAME );
    SetDllDirectory( dllPath.c_str() );
    module = LoadLibrary( _T( CASUPPORT_DLL_NAME ) );

    if( module == NULL ) {
        WLOG_ERROR( L"Failed to load %hs", CASUPPORT_DLL_NAME );
    }

    return module;
}

void UnloadModule( HMODULE module )
{
    if( module ) {
        if( !FreeLibrary( module ) ) {
            WLOG_ERROR( L"Failed to unload module" );
        }
    }

    SetDllDirectory( NULL );
}

typedef bool( *CollectUCDataFunc )( IUcLogger* logger, std::string& url, std::string& ucid, std::string& ucidToken );
bool RunCollectUCData( IUcLogger* logger, const std::wstring& dllPath, std::string& url, std::string& ucid, std::string& ucidToken )
{
    bool result = false;
    url = "";
    ucid = "";
    ucidToken = "";

    HMODULE caSupport = NULL;
    CollectUCDataFunc func = NULL;

    if( ( caSupport = LoadCaSupportDll( dllPath ) ) == NULL ) {
        LOG_ERROR( "LoadCaSupportDll failed" );
    }
    else if( ( func = ( CollectUCDataFunc )GetProcAddress( caSupport, "CollectUCData" ) ) == NULL ) {
        LOG_ERROR( "GetProcAddress CollectUCData failed" );
    }
    else {
        result = func( logger, url, ucid, ucidToken );
    }

    UnloadModule( caSupport );
    return result;
}

typedef bool( *SendEventOnUninstallBeginFunc )( IUcLogger* logger, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken );
bool RunSendEventOnUninstallBegin( IUcLogger* logger, const std::wstring& dllPath, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken )
{
    bool result = false;

    HMODULE caSupport = NULL;
    SendEventOnUninstallBeginFunc func = NULL;

    if( ( caSupport = LoadCaSupportDll( dllPath ) ) == NULL ) {
        LOG_ERROR( "LoadCaSupportDll failed" );
    }
    else if( ( func = ( SendEventOnUninstallBeginFunc )GetProcAddress( caSupport, "SendEventOnUninstallBegin" ) ) == NULL ) {
        LOG_ERROR( "GetProcAddress SendEventOnUninstallBegin failed" );
    }
    else {
        result = func( logger, url, productVersion, ucid, ucidToken );
    }

    UnloadModule( caSupport );

    return result;
}

typedef bool( *SendEventOnUninstallErrorFunc )( IUcLogger* logger, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken );
bool RunSendEventOnUninstallError( IUcLogger* logger, const std::wstring& dllPath, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken )
{
    bool result = false;

    HMODULE caSupport = NULL;
    SendEventOnUninstallErrorFunc func = NULL;

    if( ( caSupport = LoadCaSupportDll( dllPath ) ) == NULL ) {
        LOG_ERROR( "LoadCaSupportDll failed" );
    }
    else if( ( func = ( SendEventOnUninstallErrorFunc )GetProcAddress( caSupport, "SendEventOnUninstallError" ) ) == NULL ) {
        LOG_ERROR( "GetProcAddress SendEventOnUninstallError failed" );
    }
    else {
        result = func( logger, url, productVersion, ucid, ucidToken );
    }

    UnloadModule( caSupport );

    return result;
}

typedef bool( *SendEventOnUninstallCompleteFunc )( IUcLogger* logger, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken );
bool RunSendEventOnUninstallComplete( IUcLogger* logger, const std::wstring& dllPath, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken )
{
    bool result = false;

    HMODULE caSupport = NULL;
    SendEventOnUninstallCompleteFunc func = NULL;

    if( ( caSupport = LoadCaSupportDll( dllPath ) ) == NULL ) {
        LOG_ERROR( "LoadCaSupportDll failed" );
    }
    else if( ( func = ( SendEventOnUninstallCompleteFunc )GetProcAddress( caSupport, "SendEventOnUninstallComplete" ) ) == NULL ) {
        LOG_ERROR( "GetProcAddress SendEventOnUninstallComplete failed" );
    }
    else {
        result = func( logger, url, productVersion, ucid, ucidToken );
    }

    UnloadModule( caSupport );

    return result;
}

void TryKillProcessByName( const std::wstring filename )
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, NULL );
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof( pEntry );
    BOOL hRes = Process32First( hSnapShot, &pEntry );
    while( hRes )
    {
        if( filename == std::wstring( pEntry.szExeFile ) )
        {
            HANDLE hProcess = OpenProcess( PROCESS_TERMINATE, 0, ( DWORD )pEntry.th32ProcessID );
            if( hProcess != NULL )
            {
                TerminateProcess( hProcess, 9 );
                CloseHandle( hProcess );
            }
        }
        hRes = Process32Next( hSnapShot, &pEntry );
    }
    CloseHandle( hSnapShot );
}

bool MovePluginForDeletion( const std::filesystem::path& src, const std::filesystem::path& destFolder )
{
    bool rtn;
    std::srand( ( unsigned int )std::time( nullptr ) );

    std::filesystem::path destFile = destFolder;

    destFile /= std::to_wstring( std::rand() );
    destFile += L"_csccmplugin.dll";

    std::filesystem::create_directories( destFolder );
    rtn = MoveFileEx( src.c_str(), destFile.c_str(), MOVEFILE_REPLACE_EXISTING );

    if( rtn ) {
        if( !MoveFileEx( destFile.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT ) ) {
            WLOG_WARNING( L"Failed to mark %s for deletion", destFile.c_str() );
        }
    }
    else {
        WLOG_ERROR( L"Failed to move %s to %s", src.c_str(), destFile.c_str() );
    }

    return rtn;
}

void BuildCustomActionData( const std::vector<std::wstring>& propertyList, const char delim, std::wstring& str )
{
    str.clear();

    for( size_t i = 0; i < propertyList.size(); i++ ) {
        LPWSTR tempPath = NULL;
        HRESULT hr = WcaGetProperty( propertyList[ i ].c_str(), &tempPath );
        if( SUCCEEDED( hr ) ) {
            str += tempPath;
            if( ( i + 1 ) < propertyList.size() ) {
                str += L"|";
            }
        }
        else {
            WLOG_ERROR( L"WcaGetProperty failed on %s", propertyList[ i ].c_str() );
        }
    }
}
void Tokenize( const std::wstring &str, const char delim, std::vector<std::wstring>& stringList )
{
    size_t start;
    size_t end = 0;

    while( ( start = str.find_first_not_of( delim, end ) ) != std::wstring::npos ) {
        end = str.find( delim, start );
        stringList.push_back( str.substr( start, end - start ) );
    }
}
#include "pch.h"
#include "UcLogFile.h"
#include "UcLogger.h"
#include "DiagToolContainer.h"
#include "IDiagTool.h"
#include "WindowsUtilities.h"
#include "ToastNotificationPluginImpl.h"
#include "WinApiWrapper.h"
#include <tchar.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include "StringResources.h"

#define TOAST_APP_NAME L"Cisco\\Cisco Cloud Management Diagnostics"
#define TOAST_AUMI L"Cisco.CM"
#define TOAST_TIMEOUT_MS ( 5 * 60 * 1000)

std::wstring GetTranslatedText( int id )
{
    LANGID user_lang_id = GetUserDefaultUILanguage();

    if( _translationMap.find( user_lang_id ) == _translationMap.end() ) {
        user_lang_id = DEFAULT_LANG_ID;
    }

    return _translationMap[ user_lang_id ][ id ];
}

void SendToast( const std::wstring& toastMsg )
{
    WLOG_DEBUG( L"ENTER" );

    CoInitialize( NULL );
    CToastNotificationPlugin* toast = new CToastNotificationPlugin();

    if( toast ) {
        const wchar_t* msg[] = { L"Cisco Cloud Management Diagnostics", toastMsg.c_str() };
        PluginResult res = toast->SendToastNotification( ToastImageAndText02,            //Toast template type
            TOAST_AUMI,   //AppUserModelID set by the installer ("Cisco.AnyConnect")
            L"",           //path to the icon
            msg, //unicode string message array
            2 );

        WLOG_DEBUG( L"SendToastNotification result %d", res );

        Sleep( 1000 );

        delete toast;
    }
    else {
        WLOG_ERROR( L"Failed to create CToastNotificationPlugin" );
    }

    CoUninitialize();
    WLOG_DEBUG( L"Exit" );
}

void SendRebootToast()
{
    SendToast( GetTranslatedText( IDS_UPDATE_REQUIRES_REBOOT ).c_str() );
}

void SendElevationFailedToast()
{
    SendToast( GetTranslatedText( IDS_ELEVATION_REQUIRED ).c_str() );
}

DWORD GetParentPID( DWORD pid )
{
    HANDLE handle = NULL;
    PROCESSENTRY32 pe = { 0 };
    DWORD ppid = 0;
    
    pe.dwSize = sizeof( PROCESSENTRY32 );
    handle = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( handle == NULL ) {
        WLOG_ERROR( L"CreateToolhelp32Snapshot failed" );
    }
    else if( !Process32First( handle, &pe ) ) {
        WLOG_ERROR( L"Process32First failed" );
    }
    else {
        do {
            if( pe.th32ProcessID == pid ) {
                ppid = pe.th32ParentProcessID;
                break;
            }
        } while( Process32Next( handle, &pe ) );
    }

    if( handle ) {
        CloseHandle( handle );
    }

    if( ppid == 0 ) {
        WLOG_ERROR( L"Parrent Process not found for pid %d", pid );
    }

    return ( ppid );
}

bool GetProcessName( DWORD pid, std::wstring& processName )
{
    HANDLE handle = NULL;
    WCHAR fname[ MAX_PATH + 1 ] = { 0 };
    bool rtn = false;

    handle = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );
    if( handle == NULL ) {
        WLOG_ERROR( L"OpenProcess failed" );
    } 
    else {
        if( GetModuleFileNameEx( handle, NULL, fname, MAX_PATH ) ) {
            rtn = true;
            processName = fname;
        }
        else {
            WLOG_ERROR( L"GetModuleFileNameEx failed" );
        }
    }

    if( handle ) {
        CloseHandle( handle );
    }

    return ( rtn );
}

BOOL IsProcessElevated()
{
    BOOL fIsElevated = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    HANDLE hToken = NULL;

    // Open the primary access token of the process with TOKEN_QUERY. 
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Retrieve token elevation information. 
    TOKEN_ELEVATION elevation;
    DWORD dwSize;
    if (!GetTokenInformation(hToken, TokenElevation, &elevation,
        sizeof(elevation), &dwSize))
    {
        // When the process is run on operating systems prior to Windows  
        // Vista, GetTokenInformation returns FALSE with the  
        // ERROR_INVALID_PARAMETER error code because TokenElevation is  
        // not supported on those operating systems. 
        dwError = GetLastError();
        goto Cleanup;
    }

    fIsElevated = elevation.TokenIsElevated;

Cleanup:
    // Centralized cleanup for all allocated resources. 
    if (hToken)
    {
        CloseHandle(hToken);
        hToken = NULL;
    }

    return fIsElevated;
}

void RunElevated( int argc, wchar_t** argv )
{
    std::wstring parentName;
    DWORD parentPid = GetParentPID( GetCurrentProcessId() );

    if( !GetProcessName( parentPid, parentName ) ) {
        WLOG_ERROR( L"GetProcessName failed" );
    }
    else if( parentName == argv[ 0 ]) {
        WLOG_ERROR( L"Parent process is %s. Elevation likely failed", argv[ 0 ] );
        SendElevationFailedToast();
    }
    else {
        SHELLEXECUTEINFO shExInfo = { 0 };
        WCHAR swPath[ MAX_PATH + 5 ] = { 0 };
        GetModuleFileName( NULL, swPath, MAX_PATH );
        std::wstring arglist;
        for( int i = 1; i < argc; i++ ) {
            arglist += argv[ i ];
            arglist += L" ";
        }

        shExInfo.cbSize = sizeof( shExInfo );
        shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        shExInfo.hwnd = 0;
        shExInfo.lpVerb = _T( "runas" );                // Operation to perform
        shExInfo.lpFile = swPath;
        shExInfo.lpParameters = arglist.c_str();
        shExInfo.lpDirectory = 0;
        shExInfo.nShow = SW_SHOW;
        shExInfo.hInstApp = 0;

        if( ShellExecuteEx( &shExInfo ) ) {
            WLOG_DEBUG( L"Created Elevated Child Process" );
            WaitForSingleObject( shExInfo.hProcess, 5000 );
            CloseHandle( shExInfo.hProcess );
        }
    }
}

std::filesystem::path GetLogFileName()
{
    std::filesystem::path logFilename = WindowsUtilities::ResolveKnownFolderIdForCurrentUser( "FOLDERID_ProgramData" );
    WCHAR swPath[ MAX_PATH + 5 ] = { 0 };
    DWORD dwSize = GetModuleFileName( NULL, swPath, MAX_PATH );
    std::wstring modulePath = swPath;

    logFilename /= "Cisco";
    logFilename /= "Cisco Secure Client";
    logFilename /= "CM";

    if( dwSize && ( modulePath.find_last_of( '\\' ) != std::wstring::npos ) ) {
        modulePath = modulePath.substr( modulePath.find_last_of( '\\' ) + 1 );
        logFilename /= modulePath;
    }
    else {
        logFilename /= L"UnknownApplication";
    }

    logFilename += L".log";

    return logFilename;
}

int wmain( int argc, wchar_t** argv, wchar_t** envp )
{
    bool usermode = false;
    bool customOutputFile = false;
    bool customOutputDir = false;

    std::wstring outputFile;

    UcLogFile logFile;
    std::filesystem::path logFilename = GetLogFileName();
    logFile.Init( logFilename.wstring().c_str() );
    WindowsUtilities::AllowEveryoneAccessToFile( logFilename );
    
    UcLogger logger( logFile );
    logger.SetLogLevel( IUcLogger::LOG_DEBUG );
    SetUcLogger( &logger );

    WLOG_DEBUG( L"Enter" );

    for( int i = 0; i < argc; i++ ) {
        if( ( std::wstring( L"--notifyreboot" ) == argv[ i ] ) ) {
            SendRebootToast();
            return 0;
        }
        else if( ( std::wstring( L"-u" ) == argv[ i ] ) ) {
            usermode = true;
            WLOG_DEBUG( L"Running in user mode" );
        }
        else if( ( std::wstring( L"-o" ) == argv[ i ] ) ) {
            if( ( i + 1 ) < argc ) {
                i++;
                outputFile = argv[ i ];
            }
            else {
                return -1;
            }
            customOutputFile = true;
        }
        else if( ( std::wstring( L"-d" ) == argv[ i ] ) ) {
            if( ( i + 1 ) < argc ) {
                i++;
                outputFile = argv[ i ];
            }
            else {
                return -1;
            }
            customOutputDir = true;
        }
    }

    if( customOutputFile && customOutputDir ) {
        LOG_ERROR( L"Invalid parameters: -o and -d are incompatable" );
        return -1;
    }

    if( !usermode && !IsProcessElevated() ) {
        WLOG_DEBUG( L"Elevation required" );
        RunElevated( argc, argv );
        return 0;
    }

    std::vector<std::filesystem::path> additionalFiles;
    additionalFiles.push_back( logFilename );

    DiagToolContainer diagToolContainer( &additionalFiles );

    if( !customOutputDir ) {
        diagToolContainer.GetDiagTool().CreateDiagnosticPackage( outputFile );
    }
    else {
        diagToolContainer.GetDiagTool().CreateDiagnosticFiles( outputFile );
    }

    WLOG_DEBUG( L"Exit" );
    return 0;
}

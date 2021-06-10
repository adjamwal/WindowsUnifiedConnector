#include "pch.h"
#include "UcLogFile.h"
#include "UcLogger.h"
#include "DiagToolContainer.h"
#include "IDiagTool.h"
#include "WindowsUtilities.h"
#include "wintoastlib.h"
#include "WinApiWrapper.h"
#include <tchar.h>

#define TOAST_APP_NAME L"Cisco\\Cisco Unified Connector Diagnostics"
#define TOAST_AUMI L"Cisco.UC"
#define TOAST_TIMEOUT_MS 5 * 60 * 1000  // 5 minutes

class CustomHandler : public WinToastLib::IWinToastHandler {
public:
    void toastActivated() const {
        LOG_DEBUG("The user clicked in this toast");
        exit(0);
    }

    void toastActivated(int actionIndex) const {
        LOG_DEBUG("The user clicked on action #%d", actionIndex);
        if (actionIndex == 0) {
            LOG_DEBUG("Rebooting System");
            WinApiWrapper winApitWrapper;
            winApitWrapper.ExitWindowsEx(
                EWX_REBOOT | EWX_FORCEIFHUNG,
                SHTDN_REASON_MAJOR_SOFTWARE |
                SHTDN_REASON_MINOR_INSTALLATION |
                SHTDN_REASON_FLAG_PLANNED);
        }
        exit(0);
    }

    void toastDismissed(WinToastDismissalReason state) const {
        LOG_DEBUG("Enter");

        switch (state) {
        case UserCanceled:
            LOG_DEBUG("The user dismissed this toast");
            break;
        case TimedOut:
            LOG_DEBUG("The toast has timed out");
            break;
        case ApplicationHidden:
            LOG_DEBUG("The application hid the toast using ToastNotifier.hide()");
            break;
        default:
            LOG_DEBUG("Toast not activated");
            break;
        }

        exit(0);
    }

    void toastFailed() const {
        LOG_DEBUG("Error showing current toast");
        exit(0);
    }
};

void SendRebootToast()
{
    WinToastLib::WinToast* toast = WinToastLib::WinToast::instance();
    WinToastLib::WinToastTemplate::AudioOption audioOption = WinToastLib::WinToastTemplate::AudioOption::Default;
    const DWORD expiration = TOAST_TIMEOUT_MS;

    if (toast) {
        toast->setAppName(TOAST_APP_NAME);
        toast->setAppUserModelId(TOAST_AUMI);
        //Setup the shortcut first (Set AppUserModelId). Sometimes the first toast doesn't appear if this isn't already set
        //This is a no-op if the shortcut is already configured
        enum WinToastLib::WinToast::ShortcutResult result = toast->createShortcut();
        if( ( result == WinToastLib::WinToast::SHORTCUT_WAS_CHANGED ) ||
            ( result == WinToastLib::WinToast::SHORTCUT_WAS_CREATED ) ) {
            LOG_DEBUG( "Shortcut modified/created. Sleeping 5 seconds to let changes take effect" );
            Sleep( 5000 );
        }

        if (!toast->initialize()) {
            LOG_ERROR("Toast Initialization Failed");
        }
        else {
            WinToastLib::WinToastTemplate templ(WinToastLib::WinToastTemplate::Text02);
            templ.setTextField(L"A software update requires a reboot to complete. Would you like to restart Windows now?", WinToastLib::WinToastTemplate::FirstLine);
            templ.setAudioOption(audioOption);
            templ.setAttributionText(L"");

            templ.addAction(L"Yes");
            templ.addAction(L"No");

            templ.setExpiration(expiration);


            if (toast->showToast(templ, new CustomHandler()) < 0) {
                LOG_ERROR("Could not launch toast notification");
            }

            // Give the handler a chance for 15 seconds (or the expiration plus 1 second)
            Sleep((DWORD)expiration + 2000);
            LOG_ERROR("Timed Out Waiting");
        }
    }
}

#include <tlhelp32.h>
#include <Psapi.h>

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

int wmain(int argc, wchar_t** argv, wchar_t** envp)
{
    std::wstring dataDir = WindowsUtilities::GetLogDir();

    UcLogFile logFile;
    logFile.Init();

    UcLogger logger( logFile );
    logger.SetLogLevel( IUcLogger::LOG_DEBUG );
    SetUcLogger( &logger );

    WLOG_DEBUG( L"Enter" );

    if ((argc > 1) && (std::wstring( L"--notifyreboot" ) == argv[1])) {
        SendRebootToast();
        return 0;
    }

    if( !IsProcessElevated() ) {
        WLOG_DEBUG( L"Elevation required" );
        RunElevated( argc, argv );
        return 0;
    }

    DiagToolContainer diagToolContainer;

    diagToolContainer.GetDiagTool().CreateDiagnosticPackage();

    WLOG_DEBUG( L"Exit" );
    return 0;
}

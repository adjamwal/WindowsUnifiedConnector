#include "pch.h"
#include "UcLogFile.h"
#include "UcLogger.h"
#include "DiagToolContainer.h"
#include "IDiagTool.h"
#include "WindowsUtilities.h"
#include "wintoastlib.h"
#include "WinApiWrapper.h"

#define TOAST_APP_NAME L"Cisco\\Cisco Unified Connector Diagnositcs"
#define TOAST_AUMI L"Cisco.UC"

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
    const DWORD expiration = 10000;

    if (toast) {
        toast->setAppName(TOAST_APP_NAME);
        toast->setAppUserModelId(TOAST_AUMI);

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

int main( int argc, char** argv )
{
    std::wstring dataDir = WindowsUtilities::GetLogDir();

    UcLogFile logFile;
    logFile.Init();

    UcLogger logger( logFile );
    logger.SetLogLevel( IUcLogger::LOG_DEBUG );
    SetUcLogger( &logger );

    WLOG_DEBUG( L"Enter" );

    if ((argc > 0) && (std::string( "--notifyreboot" ) == argv[1])) {
        SendRebootToast();
        return 0;
    }

    DiagToolContainer diagToolContainer;

    diagToolContainer.GetDiagTool().CreateDiagnosticPackage();

    WLOG_DEBUG( L"Exit" );
    return 0;
}

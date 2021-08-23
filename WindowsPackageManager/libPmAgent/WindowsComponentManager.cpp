#include "pch.h"
#include "WindowsComponentManager.h"
#include "WindowsUtilities.h"
#include "IWinApiWrapper.h"
#include "ICodesignVerifier.h"
#include "IPackageDiscovery.h"
#include "IUserImpersonator.h"
#include "IUcLogger.h"
#include <sstream>
#include <locale>
#include <codecvt>
#include <AccCtrl.h>

#define UCSERVICE_PATH_REG_KEY L"Software\\Cisco\\SecureClient\\UnifiedConnector\\UCSERVICE"
#define DIAG_TOOL_EXE L"csc_ucdt.exe"
#define DIAG_TOOL_NOTIFY_ARG L"--notifyreboot"

WindowsComponentManager::WindowsComponentManager( IWinApiWrapper& winApiWrapper, 
    ICodesignVerifier& codesignVerifier, 
    IPackageDiscovery& packageDiscovery,
    IUserImpersonator& userImpersonator ) :
    m_winApiWrapper( winApiWrapper )
    , m_codeSignVerifier( codesignVerifier )
    , m_packageDiscovery( packageDiscovery )
    , m_userImpersonator( userImpersonator )
{
}

WindowsComponentManager::~WindowsComponentManager() { }

int32_t WindowsComponentManager::GetInstalledPackages( const std::vector<PmProductDiscoveryRules>& catalogRules, PackageInventory& packagesDiscovered )
{
    packagesDiscovered = m_packageDiscovery.DiscoverInstalledPackages( catalogRules );

    return 0;
}

int32_t WindowsComponentManager::GetCachedInventory( PackageInventory& cachedInventory )
{
    cachedInventory = m_packageDiscovery.CachedInventory();

    return 0;
}

int32_t WindowsComponentManager::InstallComponent( const PmComponent& package )
{
    return -1;
}

int32_t WindowsComponentManager::UpdateComponent( const PmComponent& package, std::string& error )
{
    int32_t ret = 0;
    CodesignStatus status = CodesignStatus::CODE_SIGNER_ERROR;

    std::filesystem::path downloadedInstallerPath = package.downloadedInstallerPath;
    downloadedInstallerPath.make_preferred();

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    if( !package.signerName.empty() ) {
        status = m_codeSignVerifier.Verify(
            converter.from_bytes(downloadedInstallerPath.u8string()),
            converter.from_bytes(package.signerName),
            SIGTYPE_DEFAULT);
    }
    else {
        status = CodesignStatus::CODE_SIGNER_SUCCESS;
    }

    if ( status == CodesignStatus::CODE_SIGNER_SUCCESS )
    {
        if ( package.installerType == "exe" )
        {
            std::string exeCmdline;
            exeCmdline = downloadedInstallerPath.filename().u8string();
            exeCmdline += " ";
            exeCmdline += package.installerArgs;

            ret = RunPackage( downloadedInstallerPath.u8string(), exeCmdline, error );
        }
        else if ( package.installerType == "msi" )
        {
            std::string msiexecFullPath;
            std::string msiCmdline = "";

            if ( WindowsUtilities::GetSysDirectory( msiexecFullPath ) )
            {
                std::string logFilePath = converter.to_bytes(WindowsUtilities::GetLogDir());
                std::string logFileName = package.productAndVersion;

                std::replace( logFileName.begin(), logFileName.end(), '/', '.' );
                logFilePath.append( "\\" ).append( logFileName ).append(".log");

                msiexecFullPath.append( "\\msiexec.exe" );

                msiCmdline = " /package \"" + downloadedInstallerPath.u8string() + "\" /quiet /L*V \"" + logFilePath + "\" " + package.installerArgs + " /norestart";

                ret = RunPackage( msiexecFullPath, msiCmdline, error );
            }
            else
            {
                error = std::string( "Failed to get system directory." );
                ret = -1;
            }
        }
        else
        {
            error = std::string( "Invalid Package Type: " + package.installerType );
            ret = -1;
        }
    }
    else
    {
        error = std::string( "Could not verify Package." );
        ret = (int32_t)status;
    }

    return ret;
}

int32_t WindowsComponentManager::UninstallComponent( const PmComponent& package )
{
    return -1;
}

int32_t WindowsComponentManager::DeployConfiguration( const PackageConfigInfo& config )
{
    int32_t ret = 0;
    CodesignStatus status = CodesignStatus::CODE_SIGNER_ERROR;

    std::string verifyFullPath = config.installLocation.generic_u8string() + "\\" + config.verifyBinPath;
    std::string verifyCmdLine = "--config-path " + config.verifyPath;
    std::string errorStr;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    if (!config.signerName.empty()) {
        status = m_codeSignVerifier.Verify(
            converter.from_bytes(verifyFullPath),
            converter.from_bytes(config.signerName),
            SIGTYPE_DEFAULT);
    }
    else {
        status = CodesignStatus::CODE_SIGNER_SUCCESS;
    }

    if( status == CodesignStatus::CODE_SIGNER_SUCCESS ) {
        ret = RunPackage( config.verifyBinPath, verifyCmdLine, errorStr );
    }
    else {
        ret = ( int32_t )status;
    }

    return ret;
}

std::string WindowsComponentManager::ResolvePath( const std::string& basePath )
{
    return WindowsUtilities::ResolvePath( basePath );
}

int32_t WindowsComponentManager::RunPackage( std::string executable, std::string cmdline, std::string& error )
{
    int32_t ret = 0;
    DWORD exit_code = 0;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring exe = converter.from_bytes( executable );
    std::wstring cmd = converter.from_bytes( cmdline );

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof( si );
    ZeroMemory( &pi, sizeof( pi ) );

    WLOG_DEBUG( L"Executing: %s, args: %s", exe.c_str(), cmd.c_str() );
    if ( m_winApiWrapper.CreateProcessW( &exe[0], &cmd[0], nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi ) )
    {
        ret = m_winApiWrapper.WaitForSingleObject( pi.hProcess, 300000 );

        if ( ret == WAIT_OBJECT_0 )
        {
            if ( m_winApiWrapper.GetExitCodeProcess( pi.hProcess, &exit_code ) )
            {
                if ( exit_code != 0 )
                {
                    ret = exit_code;
                    error = std::string( "CreateProcess GetExitCodeProcess returned: " + std::to_string( ret ) );
                }
            }
            else
            {
                ret = m_winApiWrapper.GetLastError();
                error = std::string( "Failed to get last Exit Code for update Exe. GetLastError: " + std::to_string( ret ) );
            }
        } 
        else
        {
            error = std::string( "WaitForSingleObject Failed with return value: " + std::to_string( ret ) );
        }

        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
    }
    else
    {
        ret = m_winApiWrapper.GetLastError();
        error = std::string( "Failed to run update. GetLastError: " + std::to_string( ret ) );
    }

    return ret;
}

int32_t WindowsComponentManager::FileSearchWithWildCard( const std::filesystem::path& searchPath, std::vector<std::filesystem::path>& results )
{
    return WindowsUtilities::FileSearchWithWildCard( searchPath, results );
}

void WindowsComponentManager::NotifySystemRestart()
{
    LOG_DEBUG( "Enter" );

    std::wstring diagToolDir;
    std::vector<ULONG> sessionList;

    if( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, UCSERVICE_PATH_REG_KEY, L"Path", diagToolDir ) || diagToolDir.empty() ) {
        LOG_ERROR( "Failed to get diag tool path" );
        return;
    }

    if( !m_userImpersonator.GetActiveUserSessions( sessionList ) ) {
        LOG_ERROR( "GetActiveUserSessions failed" );
    }
    else {
        for( auto& session : sessionList ) {
            if( !m_userImpersonator.RunProcessInSession(
                DIAG_TOOL_EXE,
                DIAG_TOOL_NOTIFY_ARG,
                diagToolDir,
                session
            ) ) {
                WLOG_ERROR( L"RunProcessInSession failed. Sesssion: %d Working directory %s", session, diagToolDir );
            }
        }
    }

    LOG_DEBUG( "Exit" );
}

int32_t WindowsComponentManager::ApplyBultinUsersReadPermissions( const std::filesystem::path& filePath )
{
    return WindowsUtilities::AllowBuiltinUsersReadAccessToPath( filePath.wstring() ) ? 0 : -1;
}

int32_t WindowsComponentManager::RestrictPathPermissionsToAdmins( const std::filesystem::path& filePath )
{
    return  WindowsUtilities::SetWellKnownGroupAccessToPath( filePath.wstring(), WinAuthenticatedUserSid, GENERIC_READ ) &&
            //BUILTIN\Administrators (this includes System user, see S-1-5-18 on https://docs.microsoft.com/en-us/windows/security/identity-protection/access-control/security-identifiers)
            WindowsUtilities::SetSidAccessToPath( filePath.wstring(), L"S-1-5-32-544", TRUSTEE_IS_GROUP, GENERIC_ALL ) &&
            //NT SERVICE\TrustedInstaller
            WindowsUtilities::SetSidAccessToPath( filePath.wstring(), L"S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464", TRUSTEE_IS_USER, GENERIC_ALL )
         ? 0 : -1;
}

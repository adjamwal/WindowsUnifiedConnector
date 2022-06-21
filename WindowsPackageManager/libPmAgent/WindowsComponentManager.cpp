#include "pch.h"
#include "WindowsComponentManager.h"
#include "WindowsUtilities.h"
#include "IWinApiWrapper.h"
#include "ICodesignVerifier.h"
#include "IPackageDiscovery.h"
#include "IUserImpersonator.h"
#include "IMsiApi.h"
#include "IUcLogger.h"
#include <sstream>
#include <locale>
#include <codecvt>
#include <AccCtrl.h>
#include "CmConstants.h"

#define DIAG_TOOL_EXE L"csc_cmdt.exe"
#define DIAG_TOOL_NOTIFY_ARG L"--notifyreboot"

WindowsComponentManager::WindowsComponentManager( IWinApiWrapper& winApiWrapper,
    ICodesignVerifier& codesignVerifier,
    IPackageDiscovery& packageDiscovery,
    IUserImpersonator& userImpersonator,
    IMsiApi& msiApi ) :
    m_winApiWrapper( winApiWrapper )
    , m_codeSignVerifier( codesignVerifier )
    , m_packageDiscovery( packageDiscovery )
    , m_userImpersonator( userImpersonator )
    , m_msiApi( msiApi )
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
            converter.from_bytes( downloadedInstallerPath.u8string() ),
            converter.from_bytes( package.signerName ),
            SIGTYPE_DEFAULT );
    }
    else {
        status = CodesignStatus::CODE_SIGNER_SUCCESS;
    }

    if( status == CodesignStatus::CODE_SIGNER_SUCCESS )
    {
        if( package.installerType == "exe" )
        {
            std::string exeCmdline;
            exeCmdline = downloadedInstallerPath.filename().u8string();
            exeCmdline += " ";
            exeCmdline += package.installerArgs;

            ret = RunPackage( downloadedInstallerPath.u8string(), exeCmdline, error );
        }
        else if( package.installerType == "msi" )
        {
            std::string logFilePath = converter.to_bytes(WindowsUtilities::GetLogDir());
            ret =  UpdateMsi(package, error, logFilePath, downloadedInstallerPath);
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
        ret = ( int32_t )status;
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

    if( !config.signerName.empty() ) {
        status = m_codeSignVerifier.Verify(
            converter.from_bytes( verifyFullPath ),
            converter.from_bytes( config.signerName ),
            SIGTYPE_DEFAULT );
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
    if( m_winApiWrapper.CreateProcessW( &exe[ 0 ], &cmd[ 0 ], nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi ) )
    {
        ret = m_winApiWrapper.WaitForSingleObject( pi.hProcess, 300000 );

        if( ret == WAIT_OBJECT_0 )
        {
            if( m_winApiWrapper.GetExitCodeProcess( pi.hProcess, &exit_code ) )
            {
                if( exit_code != 0 )
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

    if( !WindowsUtilities::ReadRegistryStringW( HKEY_LOCAL_MACHINE, WREG_CM_SERVICE, L"Path", diagToolDir ) || diagToolDir.empty() ) {
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
    return
        //BUILTIN\Administrators (this includes System, see https://docs.microsoft.com/en-us/windows/security/identity-protection/access-control/security-identifiers)
        WindowsUtilities::SetPathOwnership( filePath, WinBuiltinAdministratorsSid, TRUSTEE_IS_GROUP )
        && WindowsUtilities::SetWellKnownGroupAccessToPath( filePath, WinBuiltinAdministratorsSid, GENERIC_ALL, true )
        && WindowsUtilities::SetWellKnownGroupAccessToPath( filePath, WinBuiltinUsersSid, GENERIC_READ )
        ? ERROR_SUCCESS : -1;
}

int32_t WindowsComponentManager::UpdateMsi( const PmComponent& package, std::string& error, std::string &logFilePath, std::filesystem::path &downloadedInstallerPath)
{
    int32_t ret = -1;
    bool expecting1618 = true;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string msiexecFullPath;
    std::string msiCmdline = "";

    //Wait up to 5 minutes for the Windows Installer Service
    for( int i = 0; i < 30; i++ ) {
        if( !m_msiApi.IsMsiServiceReadyforInstall() ) {
            LOG_DEBUG( "Windows Installer Service is not ready. Waiting..." );
            m_winApiWrapper.Sleep( 10000 );
        }
        else {
            expecting1618 = false;
            break;
        }
    }

    if( expecting1618 ) {
        LOG_DEBUG( "Timed out waiting for Windows Installer Service. Trying install anyway. Will most likely fail" );
    }

    if (WindowsUtilities::GetSysDirectory(msiexecFullPath))
    {
        std::string logFileName = package.productAndVersion;

        std::replace(logFileName.begin(), logFileName.end(), '/', '.');
        logFilePath.append("\\").append(logFileName).append(".log");

        msiexecFullPath.append("\\msiexec.exe");

        msiCmdline = " /package \"" + downloadedInstallerPath.u8string() + "\" /quiet /L*V \"" + logFilePath + "\" " + package.installerArgs + " /norestart";

        ret = RunPackage(msiexecFullPath, msiCmdline, error);
    }
    else
    {
        error = std::string("Failed to get system directory.");
        ret = -1;
    }

    return ret;
}
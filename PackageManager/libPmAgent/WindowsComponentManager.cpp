#include "pch.h"
#include "WindowsComponentManager.h"
#include "WindowsUtilities.h"
#include <sstream>
#include <locale>
#include <codecvt>

WindowsComponentManager::WindowsComponentManager( IWinApiWrapper& winApiWrapper, 
    ICodesignVerifier& codesignVerifier, 
    IPackageDiscovery& packageDiscovery ) :
    m_winApiWrapper( winApiWrapper )
    , m_codeSignVerifier( codesignVerifier )
    , m_packageDiscovery( packageDiscovery )
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

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    CodesignStatus status = m_codeSignVerifier.Verify( 
        converter.from_bytes( package.downloadedInstallerPath ),
        converter.from_bytes( package.signerName ), 
        SIGTYPE_DEFAULT );

    if ( status == CodesignStatus::CODE_SIGNER_SUCCESS )
    {
        if ( package.installerType == "exe" )
        {
            std::string exeCmdline;
            size_t idx = package.downloadedInstallerPath.find_last_of( '\\' );
            if( idx != std::string::npos ) {
                exeCmdline = package.downloadedInstallerPath.substr( idx + 1 );
            }
            else {
                exeCmdline = package.downloadedInstallerPath;
            }

            exeCmdline += " ";
            exeCmdline += package.installerArgs;

            ret = RunPackage( package.downloadedInstallerPath, exeCmdline, error );
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

                msiCmdline = " /package \"" + package.downloadedInstallerPath + "\" /quiet /L*V \"" + logFilePath + "\" " + package.installerArgs + " /norestart";

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

    std::string verifyFullPath = config.installLocation + "\\" + config.verifyBinPath;
    std::string verifyCmdLine = "--config-path " + config.verifyPath;
    std::string errorStr;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    CodesignStatus status = m_codeSignVerifier.Verify(
        converter.from_bytes( verifyFullPath ),
        converter.from_bytes( config.signerName ),
        SIGTYPE_DEFAULT );

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

void WindowsComponentManager::InitiateSystemRestart()
{
    LOG_DEBUG( __FUNCTION__ ": Enter" );

    BOOL result = m_winApiWrapper.ExitWindowsEx(
        EWX_REBOOT | EWX_FORCEIFHUNG,
        SHTDN_REASON_MAJOR_SOFTWARE |
        SHTDN_REASON_MINOR_INSTALLATION |
        SHTDN_REASON_FLAG_PLANNED );

    if( !result )
    {
        LOG_DEBUG( __FUNCTION__ ": ExitWindowsEx() failed, GetLastError=%ld", GetLastError() );
    }

    LOG_DEBUG( __FUNCTION__ ": Exit" );
}

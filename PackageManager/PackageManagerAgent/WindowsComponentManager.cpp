#include "pch.h"
#include "WindowsComponentManager.h"
#include "WindowsUtilities.h"
#include <PmTypes.h>
#include <sstream>
#include <locale>
#include <codecvt>

WindowsComponentManager::WindowsComponentManager( IWinApiWrapper& winApiWrapper, ICodesignVerifier& codesignVerifier ) :
    m_winApiWrapper( winApiWrapper ),
    m_codeSignVerifier( codesignVerifier )
{

}

WindowsComponentManager::~WindowsComponentManager()
{

}

int32_t WindowsComponentManager::GetInstalledPackages( PmInstalledPackage* packages, size_t& packagesLen )
{
    return -1;
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
        converter.from_bytes( package.installerPath ),
        converter.from_bytes( package.signerName ), 
        SIGTYPE_DEFAULT );

    if ( status == CodesignStatus::CODE_SIGNER_SUCCESS )
    {
        if ( package.installerType == "exe" )
        {
            ret = RunPackage( package.installerPath, package.installerArgs, error );
        }
        else if ( package.installerType == "msi" )
        {
            std::string msiexecFullPath;
            std::string msiCmdline = "";

            if ( WindowsUtilities::GetSysDirectory( msiexecFullPath ) )
            {
                msiexecFullPath.append( "\\msiexec.exe" );

                msiCmdline = " /package \"" + package.installerPath + "\" /quiet";

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

std::string WindowsComponentManager::ResolvePath( const std::string& basePath, const std::string& configPath )
{
    std::string path;

    if( configPath.empty() ) {
        WLOG_ERROR( L"configPath is empty" );
    } 
    else if( basePath.empty() ) {
        // Simple test for abosolute path;
        if( configPath.find( ":\\" ) != std::string::npos ) {
            path = configPath;
        }
        else {
            WLOG_ERROR( L"basePath is empty and configPath is not absolute" );
        }
    }
    else if( basePath.find( ":\\" ) == std::string::npos ) {
        LOG_ERROR( L"basePath %s is not valid", basePath.c_str() );
    }
    else {
        path = basePath;
        if( path.back() != '\\' && configPath.front() != '\\') {
            path += "\\";
        }
        path += configPath;
    }

    LOG_DEBUG( "Path resolved to %s", path.c_str() );

    return path;
}

int32_t WindowsComponentManager::RunPackage( std::string executable, std::string cmdline, std::string& error )
{
    int32_t ret = 0;

    DWORD retlol = 0;
    DWORD exit_code = 0;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring exe = converter.from_bytes( executable );
    std::wstring cmd = converter.from_bytes( cmdline );

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof( si );
    ZeroMemory( &pi, sizeof( pi ) );

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

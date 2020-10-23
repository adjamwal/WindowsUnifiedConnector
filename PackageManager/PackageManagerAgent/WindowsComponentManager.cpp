#include "pch.h"
#include "WindowsComponentManager.h"
#include "WindowsUtilities.h"
#include <PmTypes.h>
#include <sstream>
#include <locale>
#include <codecvt>

WindowsComponentManager::WindowsComponentManager()
{

}

WindowsComponentManager::~WindowsComponentManager()
{

}

int32_t WindowsComponentManager::GetInstalledPackages( PmInstalledPackage* packages, size_t& packagesLen )
{
    return -1;
}

int32_t WindowsComponentManager::InstallComponent( const PmPackage& package )
{
    return -1;
}

int32_t WindowsComponentManager::UpdateComponent( const PmPackage& package )// todo return struct with error info
{
    LOG_ERROR( "Enter UpdateComponent");
    
    try
    {
        std::string exeFullPath;
        std::string msiexecFullPath;
        std::string cmd;

        switch ( package.Type )
        {
        case PackageType::EXE:
            //TODO just append the backslash
            exeFullPath = package.Path + package.Name;

            RunPackage( exeFullPath, package.CmdLine );

            break;
        case PackageType::MSI:
            if ( WindowsUtilities::GetSystemDirectory( msiexecFullPath ) )
            {
                msiexecFullPath.append( "\\msiexec.exe" );

                cmd = " /package \"" + package.Path + package.Name + "\" /quiet";

                RunPackage( msiexecFullPath, cmd );
            }
            else
            {
                LOG_ERROR( "Failed to get system directory." );
            }

            break;
        default:
            LOG_ERROR( "Invalid Package Type: %d", package.Type );
            break;
        }
    }
    catch ( std::exception& ex )
    {
        LOG_ERROR( "Exception during update: %s", ex.what() );
    }

    LOG_ERROR( "Exit UpdateComponent" );

    return -1;
}

int32_t WindowsComponentManager::UninstallComponent( const PmPackage& package )
{
    return -1;
}

int32_t WindowsComponentManager::DeployConfiguration( const PmPackageConfigration& config )
{
    return -1;
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

    if ( CreateProcess( &exe[0], &cmd[0], nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi ) )
    {
        ret = WaitForSingleObject( pi.hProcess, 300000 );

        if ( ret == WAIT_OBJECT_0 )
        {
            if ( GetExitCodeProcess( pi.hProcess, &exit_code ) )
            {
                if ( exit_code != 0 )
                {
                    ret = exit_code;
                    error = std::string( "CreateProcess GetExitCodeProcess returned: " + std::to_string( ret ) );
                }
            }
            else
            {
                ret = GetLastError();
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
        ret = GetLastError();
        error = std::string( "Failed to run update. GetLastError: " + std::to_string( ret ) );
    }

    return ret;
}

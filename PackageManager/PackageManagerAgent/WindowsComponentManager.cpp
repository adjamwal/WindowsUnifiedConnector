#include "pch.h"
#include "WindowsComponentManager.h"
#include <Shlobj.h>
#include <locale>
#include <codecvt>
#include <sstream>
#include <PmTypes.h>

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

int32_t WindowsComponentManager::UpdateComponent( const PmPackage& package )
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
            exeFullPath = package.Path + package.Name;

            ExecutePackage( exeFullPath, package.CmdLine );

            break;
        case PackageType::MSI:
            msiexecFullPath = GetSystemDirectory();
            msiexecFullPath.append( "\\msiexec.exe" );

            cmd = " /package \"" + package.Path + package.Name + "\" /quiet";

            ExecutePackage( msiexecFullPath, cmd );

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

void WindowsComponentManager::ExecutePackage( std::string executable, std::string cmdline )
{
    DWORD ret = 0;
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

        if ( ret != WAIT_OBJECT_0 )
        {
            throw std::exception( std::string( "WaitForSingleObject Failed with return value: " + std::to_string( ret ) ).c_str() );
        }

        if ( !GetExitCodeProcess( pi.hProcess, &exit_code ) )
        {
            throw std::exception( std::string( "Failed to get last Exit Code for update Exe. GetLastError: " + std::to_string( GetLastError() ) ).c_str() );
        }

        if ( exit_code != 0 )
        {
            throw std::exception( std::string( "CreateProcess GetExitCodeProcess returned: " + std::to_string( exit_code ) ).c_str() );
        }

        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
    }
    else
    {
        std::stringstream ss;
        ss << "Failed to run package. " 
            << "Exe: " << executable
            << " CmdLine: " << cmdline
            << " GetLastError: " << std::to_string( GetLastError() );

        throw std::exception( ss.str().c_str() );
    }
}

std::string WindowsComponentManager::GetSystemDirectory()
{
    PWSTR path = nullptr;

    HRESULT ret = SHGetKnownFolderPath( FOLDERID_System, KF_FLAG_DEFAULT, NULL, &path );
    
    if( ret != S_OK )
    {
        throw std::exception( std::string("Failed to get system directory: " + std::to_string( ret )).c_str() );
    }

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring systemPath( path );

    return converter.to_bytes( systemPath );
}

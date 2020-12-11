#include "pch.h"
#include "WindowsComponentManager.h"
#include "WindowsUtilities.h"
#include <sstream>
#include <locale>
#include <codecvt>
#include <filesystem>
#include "..\..\GlobalVersion.h"

#define IMMUNET_REG_KEY L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Immunet Protect"

WindowsComponentManager::WindowsComponentManager( IWinApiWrapper& winApiWrapper, ICodesignVerifier& codesignVerifier ) :
    m_winApiWrapper( winApiWrapper ),
    m_codeSignVerifier( codesignVerifier )
{

}

WindowsComponentManager::~WindowsComponentManager()
{

}

void WindowsComponentManager::PadBuildNumber( std::string& versionString )
{
    while ( std::count( versionString.begin(), versionString.end(), '.' ) < 3 ) {
        versionString += ".0";
    }
}

int32_t WindowsComponentManager::GetInstalledPackages( const std::vector<PmDiscoveryComponent>& discoveryList, 
    PackageInventory& packages )
{
    packages.architecture = WindowsUtilities::Is64BitWindows() ? "x64" : "x86";
    packages.platform = "win";
    
    packages.packages.push_back( BuildUcPackage() );

    try {
        packages.packages.push_back( HackBuildAmpPackage() );
    }
    catch( std::exception ex ) {
        LOG_ERROR( "Failed to build Amp Package: %s", ex.what() );
    }

    auto programList = WindowsUtilities::GetInstalledPrograms();

    for( auto &program : programList ) {
        for( auto &interestingItem : discoveryList ) {
            
            if( interestingItem.packageName == program.name ) {
                PmInstalledPackage discoveredPackage;

                LOG_DEBUG( "Found Matching package %s %s %s",
                    interestingItem.packageId.c_str(),
                    program.name.c_str(),
                    program.version.c_str()
                 );

                discoveredPackage.packageName = interestingItem.packageId;
                discoveredPackage.packageVersion = program.version;
                PadBuildNumber( discoveredPackage.packageVersion );

                packages.packages.push_back( discoveredPackage );
                //don't break. There can be one to many relationship here
            }
        }
    }

    return 0;
}

PmInstalledPackage WindowsComponentManager::BuildUcPackage()
{
    PmInstalledPackage ucPackage;
    PackageConfigInfo ucConfig;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    ucPackage.packageName = "uc";
    ucPackage.packageVersion = converter.to_bytes( STRFORMATPRODVER );
    PadBuildNumber( ucPackage.packageVersion );

    ucConfig.deleteConfig = false;

    //TODO: Get these from somewhere. Possibly registry keys
    ucConfig.path = "C:\\Program Files\\Cisco\\SecureXYZ\\Unified Connector\\Configuration\\id.json";
    ucPackage.configs.push_back( ucConfig );

    ucConfig.path = "C:\\Program Files\\Cisco\\SecureXYZ\\Unified Connector\\Configuration\\pm.json";
    ucPackage.configs.push_back( ucConfig );

    ucConfig.path = "C:\\Program Files\\Cisco\\SecureXYZ\\Unified Connector\\Configuration\\uc.json";
    ucPackage.configs.push_back( ucConfig );

    return ucPackage;
}

PmInstalledPackage WindowsComponentManager::HackBuildAmpPackage()
{
    //TODO: This should be redone in Enterprise. The Discovery component should provide information on how
    // Discover AMP
    PmInstalledPackage ampPackage;
    PackageConfigInfo ucConfig;
    std::wstring displayName;
    std::wstring displayVersion;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    
    if( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, IMMUNET_REG_KEY, L"DisplayName", displayName ) ) {
        throw( std::exception( "Failed to read AMP display name" ) );
    } 
    else if( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, IMMUNET_REG_KEY, L"DisplayVersion", displayVersion ) ) {
        throw( std::exception( "Failed to read AMP display version" ) );
    }
    else {
        if( ( displayName == L"Immunet" ) || ( displayName == L"Cisco AMP for Endpoints Connector" ) ) {
            ampPackage.packageName = "amp";
        }
        else {
            std::string error = "Unexpected display name: ";
            error += converter.to_bytes( displayName );;
            throw( std::exception( error.c_str() ) );
        }

        ampPackage.packageVersion = converter.to_bytes( displayVersion );
        PadBuildNumber( ampPackage.packageVersion );

    }
    return ampPackage;
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
            std::string exeCmdline;
            size_t idx = package.installerPath.find_last_of( '\\' );
            if( idx != std::string::npos ) {
                exeCmdline = package.installerPath.substr( idx + 1 );
            }
            else {
                exeCmdline = package.installerPath;
            }

            exeCmdline += " ";
            exeCmdline += package.installerArgs;

            ret = RunPackage( package.installerPath, exeCmdline, error );
        }
        else if ( package.installerType == "msi" )
        {
            std::string msiexecFullPath;
            std::string msiCmdline = "";

            if ( WindowsUtilities::GetSysDirectory( msiexecFullPath ) )
            {
                std::string logFilePath = converter.to_bytes(WindowsUtilities::GetDataDir());
                std::string logFileName = package.packageNameAndVersion;

                std::replace( logFileName.begin(), logFileName.end(), '/', '.' );
                logFilePath.append( "\\" ).append( logFileName ).append(".log");

                msiexecFullPath.append( "\\msiexec.exe" );

                msiCmdline = " /package \"" + package.installerPath + "\" /quiet /qn /L*V \"" + logFilePath + "\" " + package.installerArgs;

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
    size_t begin = basePath.find( "<FOLDERID_" );
    if( begin != std::string::npos ) {
        size_t end = basePath.find( ">", begin + strlen( "<FOLDERID_" ) );
        if( end != std::string::npos ) {
            begin;

            std::string knownFolder = WindowsUtilities::ResolveKnownFolderId( basePath.substr( begin + 1, end - (begin + 1 ) ) );
            if( !knownFolder.empty() ) {
                knownFolder = basePath.substr( 0, begin ) + knownFolder + basePath.substr( end + 1 );
                return knownFolder;
            }
        }
    }

    return basePath;
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

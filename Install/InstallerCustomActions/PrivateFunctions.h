#pragma once

#include <Msi.h>
#include <string>
#include <versionhelpers.h>

class IUcLogger;

#define GUID_SIZE 39

struct BuildInfo
{
    std::wstring versionString;
    int buildNumber;
};

bool StringToBuildInfo( const std::wstring& version, BuildInfo& buildInfo );
bool GetPreviousBuildVersion( BuildInfo& buildInfo, LPWSTR& productName );
std::string GetFileVersion( const std::string filename );
bool IsWindows10OrGreater( const std::string fileVersion );
bool NotifyUninstallBeginEvent( std::string ucidToken, std::wstring productVersion );
bool NotifyUninstallFailureEvent( std::string ucidToken, std::wstring productVersion );
bool NotifyUninstallEndEvent( std::string ucidToken, std::wstring productVersion );
bool ExtractResources( std::wstring& outputPath );
bool DeleteResources( const std::wstring& dllPath );
void RunTestFunction( IUcLogger* logger, const std::wstring& dllPath );
bool RunGetUcidAndToken( IUcLogger* logger, const std::wstring& dllPath, std::string& ucid, std::string& ucidToken );
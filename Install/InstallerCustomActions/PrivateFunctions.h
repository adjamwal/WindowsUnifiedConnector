#pragma once

#include <Msi.h>
#include <string>
#include <versionhelpers.h>

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

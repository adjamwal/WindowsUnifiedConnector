#pragma once

#include <Msi.h>
#include <string>
#include <versionhelpers.h>
#include <filesystem>
#include <vector>

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
bool IsArmCpu();
bool ExtractResources( std::wstring& outputPath );
bool DeleteResources( const std::wstring& dllPath );
bool RunCollectUCData( IUcLogger* logger, const std::wstring& dllPath, std::string& url, std::string& ucid, std::string& ucidToken );
bool RunSendEventOnUninstallBegin( IUcLogger* logger, const std::wstring& dllPath, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken );
bool RunSendEventOnUninstallError( IUcLogger* logger, const std::wstring& dllPath, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken );
bool RunSendEventOnUninstallComplete( IUcLogger* logger, const std::wstring& dllPath, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken );
void TryKillProcessByName( const std::wstring filename );
void BuildCustomActionData( const std::vector<std::wstring>& propertyList, const char delim, std::wstring& str );
void Tokenize( const std::wstring& str, const char delim, std::vector<std::wstring>& stringList );
bool MovePluginForDeletion( const std::filesystem::path& src, const std::filesystem::path& destFolder );

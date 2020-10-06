#pragma once

#include <string>
#include <memory>

class IPackageManager;
class IPmPlatformDependencies;
class IPMLogger;

class PmAgent
{
public:
    PmAgent( const std::wstring& configFilePath, IPmPlatformDependencies& dependencies, IPMLogger& pmLogger );
    ~PmAgent();

    int32_t Start();
    int32_t Stop();

private:
    std::wstring m_configFile;
    IPmPlatformDependencies& m_pmDependencies;
    IPMLogger& m_pmLogger;
    IPackageManager* m_PacMan;
};

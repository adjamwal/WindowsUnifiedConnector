#pragma once

#include <string>
#include <memory>

class IPackageManager;
class IPmPlatformDependencies;
class IPMLogger;

class PmAgent
{
public:
    PmAgent( const std::wstring& bsConfigFilePath, const std::wstring& pmConfigFilePath, IPmPlatformDependencies& dependencies, IPMLogger& pmLogger );
    ~PmAgent();

    int32_t Start();
    int32_t Stop();
    int32_t VerifyPmConfig( const std::wstring& pmConfigFilePath );

private:
    std::wstring m_pmConfigFile;
    std::wstring m_pmBootstrapFile;
    IPmPlatformDependencies& m_pmDependencies;
    IPMLogger& m_pmLogger;
    IPackageManager* m_PacMan;
};

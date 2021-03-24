#pragma once

#include <string>
#include <memory>

class PmAgent;
class WindowsConfiguration;
class WindowsComponentManager;
class WindowsPmDependencies;
class IPMLogger;
class ICodesignVerifier;
class IWinCertLoader;
class IWinApiWrapper;
class IPackageDiscovery;
class IPackageDiscoveryMethods;

class PmAgentContainer
{
public:
    PmAgentContainer( const std::wstring& bsConfigFilePath, const std::wstring& pmConfigFilePath );
    ~PmAgentContainer();

    PmAgent& pmAgent();

private:
    std::unique_ptr<IWinApiWrapper> m_winApiWrapper;
    std::unique_ptr<ICodesignVerifier> m_codeSignVerifer;
    std::unique_ptr<IWinCertLoader> m_certLoader;
    std::unique_ptr<IPackageDiscoveryMethods> m_discoveryMethods;
    std::unique_ptr<IPackageDiscovery> m_packageDiscovery;
    std::unique_ptr<WindowsConfiguration> m_configuration;
    std::unique_ptr<WindowsComponentManager> m_componentMgr;
    std::unique_ptr<WindowsPmDependencies> m_pmDependencies;
    std::unique_ptr<IPMLogger> m_pmLogger;

    std::unique_ptr<PmAgent> m_pmAgent;
};

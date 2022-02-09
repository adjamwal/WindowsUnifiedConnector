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
class IMsiApi;
class IPackageDiscovery;
class IPackageDiscoveryMethods;
class IUserImpersonator;
class IUtf8PathVerifier;
class ProxyContainer;

class PmAgentContainer
{
public:
    PmAgentContainer( const std::wstring& bsConfigFilePath, const std::wstring& pmConfigFilePath );
    ~PmAgentContainer();

    PmAgent& pmAgent();

private:
    std::unique_ptr<IWinApiWrapper> m_winApiWrapper;
    std::unique_ptr<IMsiApi> m_msiApi;
    std::unique_ptr<IUtf8PathVerifier> m_utf8PathVerifier;
    std::unique_ptr<ICodesignVerifier> m_codeSignVerifer;
    std::unique_ptr<IWinCertLoader> m_certLoader;
    std::unique_ptr<IPackageDiscoveryMethods> m_discoveryMethods;
    std::unique_ptr<ProxyContainer> m_proxyContainer;
    std::unique_ptr<IPackageDiscovery> m_packageDiscovery;
    std::unique_ptr<IUserImpersonator> m_userImpersonator;
    std::unique_ptr<WindowsConfiguration> m_configuration;
    std::unique_ptr<WindowsComponentManager> m_componentMgr;
    std::unique_ptr<WindowsPmDependencies> m_pmDependencies;
    std::unique_ptr<IPMLogger> m_pmLogger;

    std::unique_ptr<PmAgent> m_pmAgent;
};

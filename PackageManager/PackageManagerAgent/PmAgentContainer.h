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

class PmAgentContainer
{
public:
    PmAgentContainer( const std::wstring& configFilePath );
    ~PmAgentContainer();

    PmAgent& pmAgent();

private:
    std::unique_ptr<ICodesignVerifier> m_codeSignVerifer;
    std::unique_ptr<IWinCertLoader> m_certLoader;
    std::unique_ptr<WindowsConfiguration> m_configuration;
    std::unique_ptr<WindowsComponentManager> m_componentMgr;
    std::unique_ptr<WindowsPmDependencies> m_pmDependencies;
    std::unique_ptr<IPMLogger> m_pmLogger;

    std::unique_ptr<PmAgent> m_pmAgent;
};

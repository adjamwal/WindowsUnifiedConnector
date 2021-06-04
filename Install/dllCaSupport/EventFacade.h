#pragma once

#include "IEventFacade.h"
#include "IUcLogger.h"
#include <string>
#include <memory>

class IFileSysUtil;
class IPmHttp;
class IPmCloud;
class InstallerConfig;
class ICloudEventStorage;
class ICodesignVerifier;
class IWinCertLoader;
class IPmPlatformConfiguration;
class ICloudEventPublisher;
class ICloudEventBuilder;

class EventFacade : public IEventFacade
{
public:
    EventFacade();
    virtual ~EventFacade();

    bool SendEventOnUninstallBegin( std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken ) override;
    bool SendEventOnUninstallError( std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken ) override;
    bool SendEventOnUninstallComplete( std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken ) override;

private:
    bool SendEvent( ICloudEventBuilder& ev, std::string& url, std::string& ucid, std::string& ucidToken );

    IUcLogger* m_oldLogger;
    std::unique_ptr<IFileSysUtil> m_fileUtil;
    std::unique_ptr<IPmHttp> m_http;
    std::unique_ptr<IPmCloud> m_cloud;
    std::unique_ptr<InstallerConfig> m_config;
    std::unique_ptr<ICloudEventStorage> m_eventStorage;
    std::unique_ptr<ICodesignVerifier> m_codeSignVerifer;
    std::unique_ptr<IWinCertLoader> m_certLoader;
    std::unique_ptr<IPmPlatformConfiguration> m_winConf;
    std::unique_ptr<ICloudEventPublisher> m_eventPublisher;
};

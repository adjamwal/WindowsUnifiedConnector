#pragma once

#include "IUcidFacade.h"
#include "IUcLogger.h"
#include <string>
#include <memory>

class ICodesignVerifier;
class IWinCertLoader;
class IPmPlatformConfiguration;
class ProxyContainer;

class UcidFacade : public IUcidFacade
{
public:
    UcidFacade();
    virtual ~UcidFacade();

    bool CollectUCData( std::string& url, std::string& ucid, std::string& ucidToken ) override;

private:
    IUcLogger* m_oldLogger; 
    std::unique_ptr<ICodesignVerifier> m_codeSignVerifer;
    std::unique_ptr<IWinCertLoader> m_certLoader;
    std::unique_ptr<ProxyContainer> m_proxyContainer;
    std::unique_ptr<IPmPlatformConfiguration> m_winConf;
};

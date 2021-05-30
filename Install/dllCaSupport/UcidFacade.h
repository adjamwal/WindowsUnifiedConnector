#pragma once

#include "IUcidFacade.h"
#include "IUcLogger.h"
#include <string>
#include <memory>

class ICodesignVerifier;
class IWinCertLoader;
class IPmPlatformConfiguration;

class UcidFacade : public IUcidFacade
{
public:
    UcidFacade();
    virtual ~UcidFacade();

    bool FetchCredentials( std::string& ucid, std::string& ucidToken ) override;

private:
    IUcLogger* m_oldLogger;
    std::unique_ptr<ICodesignVerifier> m_codeSignVerifer;
    std::unique_ptr<IWinCertLoader> m_certLoader;
    std::unique_ptr<IPmPlatformConfiguration> m_winConf;
};

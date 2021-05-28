#pragma once

#include "IUcidFacade.h"
#include "WinCertLoader.h"
#include "CodesignVerifier.h"
#include "WindowsConfiguration.h"
#include <string>
#include <memory>

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
    std::unique_ptr<WindowsConfiguration> m_winConf;
};

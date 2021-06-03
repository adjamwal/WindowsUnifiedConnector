#pragma once

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "IUcidFacade.h"
#include "IUcLogger.h"
#include <string>
#include <memory>

class IFileSysUtil;
class IPmConfig;
class ICodesignVerifier;
class IWinCertLoader;
class IPmPlatformConfiguration;

class UcidFacade : public IUcidFacade
{
public:
    UcidFacade();
    virtual ~UcidFacade();

    bool CollectUCData( std::string& url, std::string& ucid, std::string& ucidToken ) override;

private:
    IUcLogger* m_oldLogger; 
    std::unique_ptr<IFileSysUtil> m_fileUtil;
    std::unique_ptr<IPmConfig> m_config;
    std::unique_ptr<ICodesignVerifier> m_codeSignVerifer;
    std::unique_ptr<IWinCertLoader> m_certLoader;
    std::unique_ptr<IPmPlatformConfiguration> m_winConf;
};

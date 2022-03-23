#pragma once

#include "IProxyVerifier.h"
#include "IPmHttp.h"
#include "ProxyInfoModel.h"
#include <atomic>
#include <mutex>

class IPmConfig;
class IProxyVerifier;
class IPmBootstrap;

class PmProxyVerifier : public IProxyVerifier
{
public:
    PmProxyVerifier( IPmHttp& testHttp, IPmConfig& pmConfig, IPmBootstrap& pmBootstrap );
    ~PmProxyVerifier();

    bool IsBusy() override;
    bool IsValidProxy( const ProxyInfoModel& testProxy ) override;
private:
    IPmHttp& m_testHttp;
    IPmConfig& m_pmConfig;
    IPmBootstrap& m_pmBootstrap;
    std::atomic_bool m_isBusy;
    std::mutex m_mutex;

    bool CanReachUrl( const std::string& url );
};

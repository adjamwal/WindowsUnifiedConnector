#pragma once

#include "ICertsAdapter.h"
#include "PmHttp.h"
#include <mutex>
#include <atomic>

class CertsAdapter : public ICertsAdapter
{
public:
    CertsAdapter();
    virtual ~CertsAdapter();

    void Initialize( IPmPlatformDependencies* dep ) override;
    PmHttpCertList GetCertsList() override;

private:
    IPmPlatformDependencies* m_dependencies;
    PmHttpCertList m_certList;
    std::atomic<bool> m_hasCerts;;
    std::mutex m_mutex;

    void InternalGetCerts();
    void InternalReleaseCerts();
};

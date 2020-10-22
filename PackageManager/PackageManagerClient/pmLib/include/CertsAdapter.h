#pragma once

#include "ICertsAdapter.h"
#include "PmHttp.h"
#include <mutex>

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

    std::mutex m_mutex;
};

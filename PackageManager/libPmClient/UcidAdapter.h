#pragma once

#include "IUcidAdapter.h"
#include "IPmPlatformDependencies.h"
#include <mutex>

class UcidAdapter : public IUcidAdapter
{
public:
    UcidAdapter();
    virtual ~UcidAdapter();

    void Initialize( IPmPlatformDependencies* dep ) override;
    std::string GetAccessToken() override;
    std::string GetIdentity() override;
    bool Refresh() override;

private:
    IPmPlatformDependencies* m_dependencies;

    std::mutex m_mutex;
};

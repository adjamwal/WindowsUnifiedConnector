#pragma once

#include "ITokenAdapter.h"
#include "IPmPlatformDependencies.h"
#include <mutex>

class TokenAdapter : public ITokenAdapter
{
public:
    TokenAdapter();
    virtual ~TokenAdapter();

    void Initialize( IPmPlatformDependencies* dep ) override;
    std::string GetAccessToken() override;
    std::string GetIdentity() override;
    bool Refresh() override;

private:
    IPmPlatformDependencies* m_dependencies;

    std::mutex m_mutex;
};

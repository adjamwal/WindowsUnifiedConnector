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
    std::string GetUcidToken() override;

private:
    IPmPlatformDependencies* m_dependencies;

    std::mutex m_mutex;
    std::string m_token;
};

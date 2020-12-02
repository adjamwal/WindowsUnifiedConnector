#pragma once

#include <string>

class IPmPlatformDependencies;

//boundary separation from the external IPmPlatformDependencies->Configuration.GetIdentityToken() for proper mocking
class IUcidAdapter
{
public:
    IUcidAdapter() {}
    virtual ~IUcidAdapter() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual std::string GetAccessToken() = 0;
    virtual std::string GetIdentity() = 0;
    virtual bool Refresh() = 0;
};

#pragma once

#include <string>

class IPmPlatformDependencies;

//boundary separation from the external IPmPlatformDependencies->Configuration.GetIdentityToken() for proper mocking
class ITokenAdapter
{
public:
    ITokenAdapter() {}
    virtual ~ITokenAdapter() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual std::string GetUcidToken() = 0;
};

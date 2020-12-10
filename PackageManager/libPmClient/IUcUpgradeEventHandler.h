#pragma once

#include <string>

class ICloudEventBuilder;
class IPmPlatformDependencies;

class IUcUpgradeEventHandler
{
public:
    IUcUpgradeEventHandler() {}
    virtual ~IUcUpgradeEventHandler() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual bool StoreUcUpgradeEvent( const std::string& event ) = 0;
    virtual bool PublishUcUpgradeEvent() = 0;
};
#pragma once

#include "IUcUpgradeEventHandler.h"
#include <mutex>

class ICloudEventPublisher;
class ICloudEventStorage;
class ICloudEventBuilder;

class UcUpgradeEventHandler : public IUcUpgradeEventHandler
{
public:
    UcUpgradeEventHandler( ICloudEventPublisher& cloudEventPublisher, 
        ICloudEventStorage& ucUpgradeEventStorage, 
        ICloudEventBuilder& eventBuilder );
    ~UcUpgradeEventHandler();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool StoreUcUpgradeEvent( const std::string& event ) override;
    bool PublishUcUpgradeEvent() override;

private:
    ICloudEventPublisher& m_cloudEventPublisher;
    ICloudEventStorage& m_ucUpgradeEventStorage;
    ICloudEventBuilder& m_eventBuilder;
    IPmPlatformDependencies* m_dependencies;
    std::mutex m_mutex;
};
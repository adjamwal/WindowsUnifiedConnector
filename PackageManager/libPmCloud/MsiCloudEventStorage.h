#pragma once
//MsiCloudEventStorage.h

#include "ICloudEventStorage.h"
#include "ICloudEventBuilder.h"
#include "IFileSysUtil.h"
#include <mutex>

class MsiCloudEventStorage : public ICloudEventStorage
{
public:
    MsiCloudEventStorage();
    ~MsiCloudEventStorage();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool SaveEvent( ICloudEventBuilder& event ) override;
    bool SaveEvent( const std::string& event ) override;
    std::vector<std::string> ReadAndRemoveEvents() override;
};
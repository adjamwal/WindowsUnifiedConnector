#pragma once

#include "ICloudEventStorage.h"
#include "ICloudEventBuilder.h"
#include "IFileUtil.h"
#include <mutex>

#define CLOUD_EVENT_STORAGE_FILE "uc_event.cache"
#define UC_UPGRADE_EVENT_STORAGE_FILE "uc_upgrade.cache"

class CloudEventStorage : public ICloudEventStorage
{
public:
    CloudEventStorage( const std::string& fileName, IFileUtil& fileUtil );
    ~CloudEventStorage();

    void Initialize( IPmPlatformDependencies* dep ) override;
    int32_t SaveEvent( ICloudEventBuilder& event ) override;
    int32_t SaveEvent( const std::string& event ) override;
    std::vector<std::string> ReadEvents() override;

private:
    IPmPlatformDependencies* m_dependencies;
    std::mutex m_mutex;
    std::string m_fullPath;
    std::string m_fileName;
    IFileUtil& m_fileUtil;
};

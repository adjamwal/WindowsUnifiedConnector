#pragma once

#include "ICloudEventStorage.h"
#include "ICloudEventBuilder.h"
#include "IFileUtil.h"
#include <mutex>

class CloudEventStorage : public ICloudEventStorage
{
public:
    CloudEventStorage( const std::string& fileName, IFileUtil& fileUtil );
    ~CloudEventStorage();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool SaveEvent( ICloudEventBuilder& event ) override;
    bool SaveEvent( const std::string& event ) override;
    std::vector<std::string> ReadAndRemoveEvents() override;

private:
    IPmPlatformDependencies* m_dependencies;
    std::mutex m_mutex;
    std::string m_fullPath;
    std::string m_fileName;
    IFileUtil& m_fileUtil;
};

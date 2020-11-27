#pragma once

#include "ICloudEventStorage.h"
#include "../../PackageManager/PmUtil/IFileUtil.h"
#include <mutex>

class CloudEventStorage : public ICloudEventStorage
{
public:
    CloudEventStorage( const std::string& fileName, IFileUtil& fileUtil );
    ~CloudEventStorage();

    int32_t SaveEvent( ICloudEventBuilder& event ) override;
    int32_t SaveEvent( const std::string& event ) override;
    std::vector<std::string> ReadEvents() override;

private:
    std::mutex m_mutex;
    std::string m_fileName;
    IFileUtil& m_fileUtil;
};

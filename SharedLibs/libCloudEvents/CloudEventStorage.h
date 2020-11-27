#pragma once

#include "ICloudEventStorage.h"
#include "../../PackageManager/PmUtil/IFileUtil.h"

class CloudEventStorage : public ICloudEventStorage
{
public:
    CloudEventStorage( const std::string& fileName, IFileUtil& fileUtil );
    ~CloudEventStorage();

    int32_t SaveEvent( ICloudEventBuilder& event ) override;
    void ReadEvents( std::vector<std::string>& events ) override;

private:
    std::string m_fileName;
    IFileUtil& m_fileUtil;
};

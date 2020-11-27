#pragma once
#include <vector>

#include "../../PackageManager/PmUtil/IFileUtil.h"
#include "ICloudEventBuilder.h"

class ICloudEventStorage
{
public:
    ICloudEventStorage() {};
    virtual ~ICloudEventStorage() {};

    virtual int32_t SaveEvent( ICloudEventBuilder& event ) = 0;
    virtual void ReadEvents( std::vector<std::string>& events ) = 0;
};

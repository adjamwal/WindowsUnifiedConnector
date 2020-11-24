#pragma once

#include "ICloudEventBuilder.h"

class ICloudEventPublisher
{
public:
    ICloudEventPublisher() {};
    virtual ~ICloudEventPublisher() {};

    virtual int32_t Publish( ICloudEventBuilder& event ) = 0;
    virtual int32_t Publish( const std::string& eventJson ) = 0;
};

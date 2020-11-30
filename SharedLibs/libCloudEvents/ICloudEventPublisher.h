#pragma once

#include "ICloudEventBuilder.h"
#include <string>

class ICloudEventPublisher
{
public:
    ICloudEventPublisher() {};
    virtual ~ICloudEventPublisher() {};

    virtual void SetToken( const std::string& token ) = 0;
    virtual int32_t Publish( ICloudEventBuilder& event ) = 0;
    virtual int32_t Publish( const std::string& eventJson ) = 0;

    virtual int32_t PublishFailedEvents() = 0;
};

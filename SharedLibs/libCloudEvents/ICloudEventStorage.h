#pragma once

#include <vector>
#include <string>

class IPmPlatformDependencies;
class ICloudEventBuilder;

class ICloudEventStorage
{
public:
    ICloudEventStorage() {};
    virtual ~ICloudEventStorage() {};

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual bool SaveEvent( ICloudEventBuilder& event ) = 0;
    virtual bool SaveEvent( const std::string& event ) = 0;
    virtual std::vector<std::string> ReadEvents() = 0;
};

#pragma once

#include <string>
#include <vector>

class IPmPlatformDependencies;
struct PmComponent;

class IManifestProcessor
{
public:
    IManifestProcessor() {}
    virtual ~IManifestProcessor() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual bool ProcessManifest( std::string checkinManifest, bool& isRebootRequired ) = 0;
};

#pragma once

#include <string>

class IPmPlatformDependencies;

class IManifestProcessor
{
public:
    IManifestProcessor() {}
    virtual ~IManifestProcessor() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual bool ProcessManifest( std::string checkinManifest, bool& isRebootRequired ) = 0;
};

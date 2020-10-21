#pragma once

#include <string>

class IManifestProcessor
{
public:
    IManifestProcessor() {}
    virtual ~IManifestProcessor() {}

    virtual bool ProcessManifest( std::string checkinManifest ) = 0;
};

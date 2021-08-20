#pragma once

#include <string>

class ICheckinManifestRetriever
{
public:
    ICheckinManifestRetriever() {}
    virtual ~ICheckinManifestRetriever() {}

    virtual std::string GetCheckinManifest( std::string payload ) = 0;
};

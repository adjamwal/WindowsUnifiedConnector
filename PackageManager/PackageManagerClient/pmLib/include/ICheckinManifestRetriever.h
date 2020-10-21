#pragma once

#include <string>

class ICheckinManifestRetriever
{
public:
    ICheckinManifestRetriever() {}
    virtual ~ICheckinManifestRetriever() {}

    virtual std::string GetCheckinManifestFrom( std::string uri ) = 0;
};

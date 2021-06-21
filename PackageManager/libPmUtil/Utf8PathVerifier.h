#pragma once

#include "IUtf8PathVerifier.h"

class Utf8PathVerifier : public IUtf8PathVerifier
{
public:
    Utf8PathVerifier();
    ~Utf8PathVerifier();

    bool IsPathValid( const std::filesystem::path& pathToVerify ) override;
    void PruneInvalidPathsFromList( std::vector<std::filesystem::path>& pathList ) override;
}; 

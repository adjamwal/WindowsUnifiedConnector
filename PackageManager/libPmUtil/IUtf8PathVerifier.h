#pragma once

#include <vector>
#include <filesystem>

class IUtf8PathVerifier
{
public:
    IUtf8PathVerifier() {}
    virtual ~IUtf8PathVerifier() {}

    virtual bool IsPathValid( const std::filesystem::path& pathToVerify ) = 0;
    virtual void PruneInvalidPathsFromList( std::vector<std::filesystem::path>& pathList ) = 0;
};
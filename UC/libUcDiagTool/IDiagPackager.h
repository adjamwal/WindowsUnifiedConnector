#pragma once

#include <vector>
#include <filesystem>

class IDiagPackager
{
public:
    virtual ~IDiagPackager() {};

    virtual void CreatePackage( const std::vector<std::filesystem::path>& fileList, const std::filesystem::path& packagePath ) = 0;
    virtual void ExtractPackage( const std::filesystem::path& packagePath, const std::filesystem::path& packageDir ) = 0;
};
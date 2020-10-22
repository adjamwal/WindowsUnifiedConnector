#pragma once

#include <string>
#include <vector>

//TODO Define these
struct PmComponent
{
    int i;
};

struct PackageConfigInfo
{
    std::string path;
    std::string sha256;
};

struct PmInstalledPackage
{
    std::string packageName;
    std::string packageVersion;
    std::vector<PackageConfigInfo> configs;
};

struct PackageInventory
{
    std::string architecture;
    std::string platform;
    std::vector<PmInstalledPackage> packages;
};


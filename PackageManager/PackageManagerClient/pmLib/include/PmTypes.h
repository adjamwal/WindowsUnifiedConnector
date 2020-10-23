#pragma once
#include <string>
#include <vector>

struct PmComponent
{
    std::string packageName;
    std::string installerUrl;
    std::string installerType;
    std::string installerArgs;
    std::string installLocation;
    std::string signerName;
    std::string installerHash;
    std::string installerPath;
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


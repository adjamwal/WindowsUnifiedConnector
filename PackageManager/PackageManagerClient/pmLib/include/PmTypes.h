#pragma once

#include <string>
#include <vector>

struct PackageConfigInfo
{
    std::string path;
    std::string sha256;
    std::string contents;
    std::string verifyPath;
};

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
    std::vector<PackageConfigInfo> configs;
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


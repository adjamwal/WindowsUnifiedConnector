#pragma once
#include <string>
#include <vector>

struct PackageConfigInfo
{
    std::string path;
    std::string sha256;
    std::string contents;
    std::string verifyBinPath;
    std::string verifyPath;
    std::string installLocation;
    std::string signerName;
    std::string forComponentID; // e.g. 'AMP/1.0.0'
    bool deleteConfig;
};

struct PmComponent
{
    std::string packageNameAndVersion;
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

struct PmDiscoveryConfigurable
{
    std::string path;
    int max_instances;
    int min_instances;
    std::vector<std::string> formats;
};

struct PmDiscoveryComponent
{
    std::string packageId;
    std::string packageName;
    std::string packageVersion;
    std::vector<PmDiscoveryConfigurable> configurables;
};

struct PmProductDiscoveryConfigurable
{
    std::string path;
    int max_instances;
    bool required;
    std::vector<std::string> formats;
};

struct PmProductDiscoveryMsiMethod
{
    std::string type;
    std::string name;
    std::string vendor;
};

struct PmProductDiscoveryRegKeyType
{
    std::string key;
    std::string type;
};

struct PmProductDiscoveryRegistryMethod
{
    std::string type;
    PmProductDiscoveryRegKeyType install;
    PmProductDiscoveryRegKeyType version;
};

struct PmProductDiscoveryRules
{
    std::string product;
    std::vector<PmProductDiscoveryConfigurable> configurables;
    std::vector<PmProductDiscoveryMsiMethod> msi_discovery;
    std::vector<PmProductDiscoveryRegistryMethod> reg_discovery;
};

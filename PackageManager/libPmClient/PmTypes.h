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
    std::string forProductAndVersion; // e.g. 'AMP/1.0.0'
    bool deleteConfig;
};

struct PmComponent
{
    std::string productAndVersion; //e.g. "uc/1.0.0.150"
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
    std::string product;
    std::string version;
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

struct PmProductDiscoveryRegKeyDef
{
    std::string key;
    std::string type;
};

struct PmProductDiscoveryRegistryMethod
{
    std::string type;
    PmProductDiscoveryRegKeyDef install;
    PmProductDiscoveryRegKeyDef version;
};

struct PmProductDiscoveryMsiUpgradeCodeMethod
{
    std::string type;
    std::string upgradeCode;
};

struct PmProductDiscoveryRules
{
    std::string product; //e.g. "uc"
    std::vector<PmProductDiscoveryConfigurable> configurables;
    std::vector<PmProductDiscoveryMsiUpgradeCodeMethod> msiUpgradeCode_discovery;
    std::vector<PmProductDiscoveryMsiMethod> msi_discovery;
    std::vector<PmProductDiscoveryRegistryMethod> reg_discovery;
};

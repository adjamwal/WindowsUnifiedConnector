#pragma once

#include "IPackageInventoryProvider.h"

struct PackageConfigInfo
{
    std::string path;
    std::string sha256;
};

struct PackageDetectionInfo
{
    std::string packageName;
    std::string packageVersion;
    std::vector<PackageConfigInfo> configs;
};

struct PackageInventory
{
    std::string architecture;
    std::string platform;
    std::vector<PackageDetectionInfo> packages;
};

class PackageInventoryProvider : public IPackageInventoryProvider
{
public:
    PackageInventoryProvider();
    virtual ~PackageInventoryProvider();

    bool GetInventory( PackageInventory& inventory ) override;
};

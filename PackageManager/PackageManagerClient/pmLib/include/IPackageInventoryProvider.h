#pragma once

#include <string>
#include <vector>

struct PackageConfigInfo;
struct PmInstalledPackage;
struct PackageInventory;

class IPackageInventoryProvider
{
public:
    IPackageInventoryProvider() {}
    virtual ~IPackageInventoryProvider() {}

    virtual bool GetInventory( PackageInventory& inventory) = 0;
};

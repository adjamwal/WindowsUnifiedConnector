#pragma once

#include <string>
#include <vector>

class IPmPlatformDependencies;

struct PackageConfigInfo;
struct PmInstalledPackage;
struct PackageInventory;

class IPackageInventoryProvider
{
public:
    IPackageInventoryProvider() {}
    virtual ~IPackageInventoryProvider() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual bool GetInventory( PackageInventory& inventory) = 0;
};

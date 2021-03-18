#pragma once

#include <vector>

struct PmDiscoveryComponent;
struct PackageInventory;
struct PmInstalledPackage;
struct PmProductDiscoveryRules;

class IPackageDiscovery
{
public:
    IPackageDiscovery() {};
    virtual ~IPackageDiscovery() {};

    virtual PackageInventory GetInstalledPackages( 
        const std::vector<PmDiscoveryComponent>& discoveryList,
        const std::vector<PmProductDiscoveryRules>& catalogRules
    ) = 0;
};

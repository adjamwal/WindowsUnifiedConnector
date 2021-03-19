#pragma once

#include <vector>

struct PackageInventory;
struct PmProductDiscoveryRules;

class IPackageDiscovery
{
public:
    IPackageDiscovery() {};
    virtual ~IPackageDiscovery() {};

    virtual PackageInventory DiscoverInstalledPackages( const std::vector<PmProductDiscoveryRules>& catalogRules ) = 0;
    virtual PackageInventory CachedInventory() = 0;
};

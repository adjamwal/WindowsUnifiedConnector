#pragma once

#include <vector>

struct PmDiscoveryComponent;
struct PackageInventory;
struct PmInstalledPackage;

class IPackageDiscovery
{
public:
    IPackageDiscovery() {};
    virtual ~IPackageDiscovery() {};

    virtual PackageInventory GetInstalledPackages( const std::vector<PmDiscoveryComponent>& discoveryList ) = 0;
};
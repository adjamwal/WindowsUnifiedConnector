#pragma once

#include "IPackageDiscovery.h"
#include <string>

class PackageDiscovery : public IPackageDiscovery
{
public:
    PackageDiscovery();
    ~PackageDiscovery();

    PackageInventory GetInstalledPackages( const std::vector<PmDiscoveryComponent>& discoveryList ) override;

private:
    PmInstalledPackage BuildUcPackage();
    PmInstalledPackage HackBuildAmpPackage();
    void PadBuildNumber( std::string& versionString );
};
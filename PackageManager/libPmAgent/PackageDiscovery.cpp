#include "pch.h"
#include "PackageDiscovery.h"
#include "PmTypes.h"
#include "IUcLogger.h"
#include "StringUtil.h"
#include "WindowsUtilities.h"
#include "PackageDiscoveryMethods.h"
#include <codecvt>
#include <regex>
#include "..\..\GlobalVersion.h"

PackageDiscovery::PackageDiscovery( IPackageDiscoveryMethods& methods )
    : m_methods( methods )
{
}

PackageDiscovery::~PackageDiscovery()
{
}

PackageInventory PackageDiscovery::DiscoverInstalledPackages( const std::vector<PmProductDiscoveryRules>& catalogRules )
{
    PackageInventory inventory;

    inventory.architecture = WindowsUtilities::Is64BitWindows() ? "x64" : "x86";
    inventory.platform = "win";

    // LOG_DEBUG( "Missing product discovery rules: package %s, product %s", lookupProduct.packageName.c_str(), lookupProduct.packageProduct.c_str() );

    for( auto& lookupProduct : catalogRules )
    {
        std::vector<PmInstalledPackage> detectedInstallations;
        ApplyDiscoveryMethods( lookupProduct, detectedInstallations );

        for( auto& detectedItem : detectedInstallations )
        {
            inventory.packages.push_back( detectedItem );
        }
    }

    //TODO: review usage - do we need a mutex lock here?
    m_lastDetectedPackages = inventory;

    return inventory;
}

PackageInventory PackageDiscovery::CachedInventory()
{
    //TODO: review usage - do we need a mutex lock here?
    return m_lastDetectedPackages;
}

void PackageDiscovery::ApplyDiscoveryMethods( const PmProductDiscoveryRules& lookupProduct,
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    for( auto msiRule : lookupProduct.msi_discovery )
    {
        m_methods.DiscoverByMsi( lookupProduct, msiRule, detectedInstallations );
    }
    for( auto regRule : lookupProduct.reg_discovery )
    {
        m_methods.DiscoverByRegistry( lookupProduct, regRule, detectedInstallations );
    }
}

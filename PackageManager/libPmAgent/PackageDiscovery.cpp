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
#include <StringUtil.h>

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

    for( auto& lookupProduct : catalogRules )
    {
        std::vector<PmInstalledPackage> detectedInstallations;
        ApplyDiscoveryMethods( lookupProduct, detectedInstallations );

        for( auto& detectedItem : detectedInstallations )
        {
            inventory.packages.push_back( detectedItem );
        }
    }

    m_lastDetectedPackages = inventory;

    return inventory;
}

PackageInventory PackageDiscovery::CachedInventory()
{
    return m_lastDetectedPackages;
}

void PackageDiscovery::ApplyDiscoveryMethods( const PmProductDiscoveryRules& lookupProduct,
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    for ( auto upgradeCodeRule : lookupProduct.msiUpgradeCode_discovery ) {
        m_methods.DiscoverByMsiUpgradeCode( lookupProduct, upgradeCodeRule, detectedInstallations );
        if ( !detectedInstallations.empty() ) {
            return;
        }
    }

    for( auto msiRule : lookupProduct.msi_discovery )
    {
        m_methods.DiscoverByMsi( lookupProduct, msiRule, detectedInstallations );
        if ( !detectedInstallations.empty() ) {
            return;
        }
    }

    for( auto regRule : lookupProduct.reg_discovery )
    {
        m_methods.DiscoverByRegistry( lookupProduct, regRule, detectedInstallations );
        if ( !detectedInstallations.empty() ) {
            return;
        }
    }
}

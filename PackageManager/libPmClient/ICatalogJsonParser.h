#pragma once

#include <string>
#include <vector>

class IPmPlatformDependencies;
struct PmProductDiscoveryRules;

#define UC_CATALOG_KEY_PRODUCTS "products"
#define UC_CATALOG_KEY_CONFIGURABLES "configurables"
#define UC_CATALOG_KEY_FORMATS "formats"

#define UC_CATALOG_DISCOVERY_TYPE_MSI "msi"
#define UC_CATALOG_DISCOVERY_TYPE_REGISTRY "registry"
#define UC_CATALOG_DISCOVERY_TYPE_MSI_UPGRADE_CODE "msi_upgrade_code"

class ICatalogJsonParser
{
public:
    ICatalogJsonParser() {}
    virtual ~ICatalogJsonParser() {}

    virtual bool Parse( const std::string json, std::vector<PmProductDiscoveryRules>& returnCatalogDataset ) = 0;
    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
};

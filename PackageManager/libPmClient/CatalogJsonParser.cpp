#include "CatalogJsonParser.h"
#include "PmTypes.h"
#include "IUcLogger.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include <vector>

#define UC_CATALOG_KEY_PRODUCTS "products"
#define UC_CATALOG_KEY_CONFIGURABLES "configurables"
#define UC_CATALOG_KEY_FORMATS "formats"

#define UC_CATALOG_DISCOVERY_TYPE_MSI "msi"
#define UC_CATALOG_DISCOVERY_TYPE_REGISTRY "registry"
#define UC_CATALOG_DISCOVERY_TYPE_MSI_UPGRADE_CODE "msi_upgrade_code"

CatalogJsonParser::CatalogJsonParser() :
    m_dependencies( nullptr )
{

}

CatalogJsonParser::~CatalogJsonParser()
{

}

void CatalogJsonParser::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_dependencies = dep;
}

bool CatalogJsonParser::Parse( const std::string json, std::vector<PmProductDiscoveryRules>&returnCatalogDataset )
{
    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root, packages;
    std::string jsonError;

    try
    {
        returnCatalogDataset.clear();

        if( json.empty() ) {
            LOG_ERROR( "Catalog Json contents is empty" );
            return false;
        }
        else if( !jsonReader->parse( json.c_str(), json.c_str() + json.length(), &root, &jsonError ) ) {
            LOG_ERROR( "Catalog Json Parse error %s", jsonError.c_str() );
            return false;
        }
        else if( !root.isMember( UC_CATALOG_KEY_PRODUCTS ) || !root[ UC_CATALOG_KEY_PRODUCTS ].isArray() ) {
            LOG_ERROR( "Package array not found in Catalog Json" );
            return false;
        }
    }
    catch ( ... ) {
        LOG_ERROR( "Exception error parsing Catalog json" );
        return false;
    }

    packages = root[ UC_CATALOG_KEY_PRODUCTS ];
    for( Json::Value pkg : packages )
    {
        try {
            PmProductDiscoveryRules catalogEntry;
            bool validDiscoveryMechanismFound = false;
            catalogEntry.product = pkg[ "product" ].asString();
            for ( Json::Value discovery : pkg[ "discovery" ] ) {
                if ( discovery[ "type" ].asString() == UC_CATALOG_DISCOVERY_TYPE_MSI_UPGRADE_CODE ) {
                    ParseMsiUpgradeCodeDiscovery( discovery, catalogEntry.msiUpgradeCode_discovery );
                    validDiscoveryMechanismFound = true;
                }
                else if ( discovery[ "type" ].asString() == UC_CATALOG_DISCOVERY_TYPE_MSI ) {
                    ParseMsiDiscovery( discovery, catalogEntry.msi_discovery );
                    validDiscoveryMechanismFound = true;
                }
                    else if ( discovery[ "type" ].asString() == UC_CATALOG_DISCOVERY_TYPE_REGISTRY ) {
                    ParseRegistryDiscovery( discovery, catalogEntry.reg_discovery );
                    validDiscoveryMechanismFound = true;
                }
                    else {
                    LOG_DEBUG( "Ignoring Unknown discovery mechanism %s", discovery[ "type" ].asCString() );
                }
            }

            ParseConfigurables( pkg, catalogEntry.configurables );
            returnCatalogDataset.push_back( catalogEntry );
        }
        catch ( std::exception& e ) {
            LOG_ERROR( "Expection: %s", e.what() );
        }
    }

    return true;
}

void CatalogJsonParser::ParseConfigurables( const Json::Value& pkgValue, std::vector<PmProductDiscoveryConfigurable>& returnPkgConfigs )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    returnPkgConfigs.clear();

    if( !pkgValue.isMember( UC_CATALOG_KEY_CONFIGURABLES ) || !pkgValue[ UC_CATALOG_KEY_CONFIGURABLES ].isArray() )
    {
        return;
    }

    for( Json::Value cfg : pkgValue[ UC_CATALOG_KEY_CONFIGURABLES ] ) {
        std::vector<std::string> formats;

        ParseConfigFormats( cfg, formats );
        if( formats.size() == 0 ) throw std::exception("Configurable must have at least one valid format");
        
        PmProductDiscoveryConfigurable configEntry
        {
            std::filesystem::u8path( m_dependencies->ComponentManager().ResolvePath( cfg["path"].asString() ) ),
            std::filesystem::u8path( cfg[ "path" ].asString() ),
            cfg[ "max_instances" ].asUInt(),
            cfg[ "required" ].asBool(),
            formats
        };
        returnPkgConfigs.push_back( configEntry );
    }
}

void CatalogJsonParser::ParseConfigFormats( const Json::Value& pkgConfigValue, std::vector<std::string>& returnFormats )
{
    returnFormats.clear();

    if( !pkgConfigValue.isMember( UC_CATALOG_KEY_FORMATS ) || !pkgConfigValue[ UC_CATALOG_KEY_FORMATS ].isArray() )
    {
        return;
    }

    for( Json::Value fmt : pkgConfigValue[ UC_CATALOG_KEY_FORMATS ] ) {
        returnFormats.push_back( fmt.asString() );
    }
}

void CatalogJsonParser::ParseMsiDiscovery( const Json::Value & msiValue, std::vector<PmProductDiscoveryMsiMethod>&returnMsi )
{
    PmProductDiscoveryMsiMethod msiDiscovery;
    
    if ( msiValue[ "name" ].isString() && msiValue[ "vendor" ].isString() ) {
        msiDiscovery.type = UC_CATALOG_DISCOVERY_TYPE_MSI;
        msiDiscovery.name = msiValue[ "name" ].asString();
        msiDiscovery.vendor = msiValue[ "vendor" ].asString();

        returnMsi.push_back( msiDiscovery );
    }
    else {
        LOG_ERROR( "Failed to parse msi discovery json: %s", msiValue.asCString() );
        throw std::exception( "Invalid msi discovery json" );
    }
}

void CatalogJsonParser::ParseRegistryDiscovery( const Json::Value & regValue, std::vector<PmProductDiscoveryRegistryMethod>&returnRegistry )
{
    PmProductDiscoveryRegistryMethod regDiscovery;
  
    if ( regValue.isMember( "install" ) && regValue[ "install" ][ "key" ].isString() &&
            regValue.isMember( "version" ) && regValue[ "version" ][ "key" ].isString() ) {
        regDiscovery.type = UC_CATALOG_DISCOVERY_TYPE_REGISTRY;
        
        regDiscovery.install.key = regValue[ "install" ][ "key" ].asString();
        if ( regValue[ "install" ].isMember( "type" ) ) {
            regDiscovery.install.type = regValue[ "install" ][ "type" ].asString();
        }

        regDiscovery.version.key = regValue[ "version" ][ "key" ].asString();
        if ( regValue[ "version" ].isMember( "type" ) ) {
            regDiscovery.version.type = regValue[ "version" ][ "type" ].asString();
        }

        returnRegistry.push_back( regDiscovery );
    }
    else {
        LOG_ERROR( "Failed to parse registry discovery json: %s", regValue.asCString() );
        throw std::exception( "Invalid registry discovery json" );
    }
}

void CatalogJsonParser::ParseMsiUpgradeCodeDiscovery( const Json::Value& msiUpgardeValue, std::vector<PmProductDiscoveryMsiUpgradeCodeMethod>& returnMsi )
{
    PmProductDiscoveryMsiUpgradeCodeMethod msiUpgradeDiscovery;

    if ( msiUpgardeValue[ "code" ].isString() ) {
        msiUpgradeDiscovery.type = UC_CATALOG_DISCOVERY_TYPE_MSI_UPGRADE_CODE;
        msiUpgradeDiscovery.upgradeCode = "{" + msiUpgardeValue[ "code" ].asString() + "}";
        for ( auto& c : msiUpgradeDiscovery.upgradeCode ) {
            c = toupper( c );
        }
        returnMsi.push_back( msiUpgradeDiscovery );
    }
    else {
        LOG_ERROR( "Failed to parse msi upgrade code discovery json: %s", msiUpgardeValue.asCString() );
        throw std::exception( "Invalid msi upgrade code discovery json" );
    }
}
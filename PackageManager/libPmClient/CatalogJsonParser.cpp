#include "CatalogJsonParser.h"
#include "PmTypes.h"
#include "IUcLogger.h"
#include <vector>

#define UC_CATALOG_KEY_PACKAGES "packages"
#define UC_CATALOG_KEY_CONFIGURABLES "configurables"
#define UC_CATALOG_KEY_FORMATS "formats"

bool CatalogJsonParser::Parse( const std::string json, std::vector<PmDiscoveryComponent>& returnCatalogDataset )
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
        else if( !root.isMember( UC_CATALOG_KEY_PACKAGES ) || !root[ UC_CATALOG_KEY_PACKAGES ].isArray() ) {
            LOG_ERROR( "Package array not found in Catalog Json" );
            return false;
        }

        packages = root[ UC_CATALOG_KEY_PACKAGES ];
        for( Json::Value pkg : packages )
        {
            std::vector<PmDiscoveryConfigurable> pkgConfigs;
            ParsePackageConfigurables( pkg, pkgConfigs );

            PmDiscoveryComponent catalogEntry = {
                    pkg[ "name" ].asString(),
                    pkg[ "product" ].asString(),
                    pkg[ "version" ].asString(),
                    pkgConfigs
            };

            returnCatalogDataset.push_back( catalogEntry );
        }
    }
    catch( ... )
    {
        LOG_ERROR( "Exception error parsing Catalog json" );
        return false;
    }

    return true;
}

void CatalogJsonParser::ParsePackageConfigurables( const Json::Value& pkgValue, std::vector<PmDiscoveryConfigurable>& returnPkgConfigs )
{
    returnPkgConfigs.clear();

    if( !pkgValue.isMember( UC_CATALOG_KEY_CONFIGURABLES ) || !pkgValue[ UC_CATALOG_KEY_CONFIGURABLES ].isArray() )
    {
        return;
    }

    for( Json::Value cfg : pkgValue[ UC_CATALOG_KEY_CONFIGURABLES ] ) {
        std::vector<std::string> formats;

        ParseConfigFormats( cfg, formats );
        if( formats.size() == 0 ) throw new std::exception("Configurable must have at least one valid format");

        PmDiscoveryConfigurable configEntry
        {
            cfg[ "path" ].asString(),
            cfg[ "max_instances" ].asInt(),
            cfg[ "min_instances" ].asInt(),
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

bool CatalogJsonParser::ParseProductRules( const std::string json, std::vector<PmProductDiscoveryRules>& returnProductRules )
{
    //TODO: parse products
}
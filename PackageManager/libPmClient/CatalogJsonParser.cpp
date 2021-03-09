#include "CatalogJsonParser.h"
#include "PmTypes.h"
#include "IUcLogger.h"
#include <json/json.h>
#include <vector>

#define UC_CATALOG_KEY_PACKAGES "packages"
#define UC_CATALOG_KEY_CONFIGURABLES "configurables"

bool CatalogJsonParser::Parse( const std::string json, std::vector<PmDiscoveryComponent>& catalogDataset )
{
    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root, packages;
    std::string jsonError;

    try
    {
        catalogDataset.clear();

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
            if( pkg.isMember( UC_CATALOG_KEY_CONFIGURABLES ) || pkg[ UC_CATALOG_KEY_CONFIGURABLES ].isArray() )
            {
                for( Json::Value cfg : pkg[ UC_CATALOG_KEY_CONFIGURABLES ] ) {
                    PmDiscoveryConfigurable configEntry
                    {
                        cfg[ "path" ].asString(),
                        cfg[ "formats" ].asString(),
                        cfg[ "max_instances" ].asInt(),
                        cfg[ "min_instances" ].asInt()
                    };
                    pkgConfigs.push_back( configEntry );
                }
            }

            PmDiscoveryComponent catalogEntry = {
                    pkg[ "name" ].asString(),
                    pkg[ "product" ].asString(),
                    pkg[ "version" ].asString(),
                    pkgConfigs
            };

            catalogDataset.push_back( catalogEntry );
        }
    }
    catch( ... )
    {
        LOG_ERROR( "Exception error parsing Catalog json" );
        return false;
    }

    return true;
}

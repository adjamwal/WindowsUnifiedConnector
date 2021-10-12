#include "CheckinFormatter.h"
#include "PackageInventoryProvider.h"
#include <json/json.h>

#include <sstream>

CheckinFormatter::CheckinFormatter()
{
}

CheckinFormatter::~CheckinFormatter()
{
}

std::string CheckinFormatter::GetJson( PackageInventory& inventory )
{
    Json::Value root;

    root[ "arch" ] = inventory.architecture;
    root[ "platform" ] = inventory.platform;

    if( inventory.packages.size() ) {
        Json::Value& packages = root[ "installed" ];
        for( int i = 0; i < ( int )inventory.packages.size(); i++ ) {
            Json::Value package;
            PmInstalledPackage& packageDetection = inventory.packages[ i ];

            package[ "package" ] = packageDetection.product + "/" + packageDetection.version;
            if( packageDetection.configs.size() ) {
                Json::Value& configs = package[ "configs" ];
                for( int j = 0; j < ( int )packageDetection.configs.size(); j++ ) {
                    Json::Value config;
                    PackageConfigInfo& packageConfig = packageDetection.configs[ j ];
                    config[ "path" ] = packageConfig.unresolvedCfgPath.generic_u8string();
                    if( !packageConfig.unresolvedDeployPath.empty() )
                    {
                        config[ "deploy_path" ] = packageConfig.unresolvedDeployPath.generic_u8string();
                    }
                    config[ "sha256" ] = packageConfig.sha256;

                    configs[ j ] = config;
                }
            }
            packages[ i ] = package;
        }
    }

    return Json::writeString( Json::StreamWriterBuilder(), root );
}

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
        Json::Value& packages = root[ "packages" ];
        for( int i = 0; i < inventory.packages.size(); i++ ) {
            Json::Value package;
            PmInstalledPackage& packageDetection = inventory.packages[ i ];

            package[ "package" ] = packageDetection.packageName + "/" + packageDetection.packageVersion;
            if( packageDetection.configs.size() ) {
                Json::Value& configs = package[ "configs" ];
                for( int j = 0; j < packageDetection.configs.size(); j++ ) {
                    Json::Value config;
                    PackageConfigInfo& packageConfig = packageDetection.configs[ j ];
                    config[ "path" ] = packageConfig.path;
                    config[ "sha256" ] = packageConfig.sha256;

                    configs[ j ] = config;
                }
            }
            packages[ i ] = package;
        }
    }

    return Json::writeString( Json::StreamWriterBuilder(), root );
}

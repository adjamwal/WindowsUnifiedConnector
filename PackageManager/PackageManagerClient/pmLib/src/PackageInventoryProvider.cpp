#include "PackageInventoryProvider.h"

PackageInventoryProvider::PackageInventoryProvider()
{

}

PackageInventoryProvider::~PackageInventoryProvider()
{

}

bool PackageInventoryProvider::GetInventory( PackageInventory& inventory )
{
    PackageInventory detectedPackages( { 0 } );

    detectedPackages.architecture = "x64";
    detectedPackages.platform = "win";

    PmInstalledPackage packageInfo( { 0 } );
    packageInfo.packageName = "UC";
    packageInfo.packageVersion = "1.0.0";

    PackageConfigInfo packageConfig( { 0 } );
    packageConfig.path = "uc.json";
    packageConfig.sha256 = "92753fde4682cf87d2a3c2af46e2f7dd7c2988863eca8390f79d5fb3723a5d2f";
    packageInfo.configs.push_back(packageConfig);

    packageConfig.path = "pm.json";
    packageConfig.sha256 = "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4";
    packageInfo.configs.push_back( packageConfig );

    packageConfig.path = "id.json";
    packageConfig.sha256 = "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3";
    packageInfo.configs.push_back( packageConfig );

    detectedPackages.packages.push_back( packageInfo );

    inventory = detectedPackages;
    return true;
}

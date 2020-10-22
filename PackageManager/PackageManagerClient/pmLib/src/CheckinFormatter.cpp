#include "CheckinFormatter.h"
#include "PackageInventoryProvider.h"

#include <sstream>

CheckinFormatter::CheckinFormatter()
{
}

CheckinFormatter::~CheckinFormatter()
{
}

std::string CheckinFormatter::GetJson( PackageInventory& inventory )
{
    std::stringstream ss;

    ss << "{";
    ss << "\"arch\": \"" << inventory.architecture << "\",";
    ss << "\"platform\": \"" << inventory.platform << "\",";
    
    ss << "\"packages\": [";
    for each( PmInstalledPackage packageDetection in inventory.packages )
    {
        ss << "{";
        ss << "\"package\": \"" << packageDetection.packageName << "/" << packageDetection.packageVersion << "\",";

        ss << "\"configs\": [";
        for each( PackageConfigInfo packageConfig in packageDetection.configs )
        {
            ss << "{";
            ss << "\"path\": \"" << packageConfig.path << "\",";
            ss << "\"sha256\": \"" << packageConfig.sha256 << "\"";
            ss << "},";
        }
        ss << "]";
        ss << "},";
    }
    ss << "]";
    ss << "}";

    return ss.str();
}

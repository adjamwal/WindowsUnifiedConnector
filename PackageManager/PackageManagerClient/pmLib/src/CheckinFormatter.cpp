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
    bool firstPackage = true;
    for each( PmInstalledPackage packageDetection in inventory.packages )
    {
        if( !firstPackage ) ss << ",";
        ss << "{";
        ss << "\"package\": \"" << packageDetection.packageName << "/" << packageDetection.packageVersion << "\",";

        ss << "\"configs\": [";
        bool firstConfig = true;
        for each( PackageConfigInfo packageConfig in packageDetection.configs )
        {
            if( !firstConfig ) ss << ",";
            ss << "{";
            ss << "\"path\": \"" << packageConfig.path << "\",";
            ss << "\"sha256\": \"" << packageConfig.sha256 << "\"";
            ss << "}";
            firstConfig = false;
        }
        ss << "]";
        ss << "}";
        firstPackage = false;
    }
    ss << "]";
    ss << "}";

    return ss.str();
}

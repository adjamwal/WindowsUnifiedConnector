#include "PackageInventoryProvider.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IFileSysUtil.h"
#include "ISslUtil.h"
#include "PmLogger.h"

PackageInventoryProvider::PackageInventoryProvider( IFileSysUtil& fileUtil, ISslUtil& sslUtil ) :
    m_fileUtil( fileUtil )
    , m_sslUtil( sslUtil )
    , m_dependencies( nullptr )
{

}

PackageInventoryProvider::~PackageInventoryProvider()
{

}

void PackageInventoryProvider::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_dependencies = dep;
}

bool PackageInventoryProvider::GetInventory( PackageInventory& inventory )
{
    bool rtn = false;
    PackageInventory packagesDiscovered;

    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_dependencies ) {
        return false;
    }

    if( m_dependencies->ComponentManager().GetInstalledPackages( m_catalogRules, packagesDiscovered ) == 0 )
    {
        for( auto& package : packagesDiscovered.packages )
        {
            for( auto& configFile : package.configs )
            {
                std::optional<std::string> sha256 = std::nullopt;

                if( !configFile.deployPath.empty() )
                {
                    sha256 = m_sslUtil.CalculateSHA256( configFile.deployPath );
                    if( sha256.has_value() )
                    {
                        configFile.sha256 = sha256.value();
                    }
                }

                if( !sha256.has_value() )
                {
                    sha256 = m_sslUtil.CalculateSHA256( configFile.cfgPath );
                    if( sha256.has_value() )
                    {
                        configFile.sha256 = sha256.value();
                    }
                }
            }
        }

        inventory = packagesDiscovered;
        rtn = true;
    }

    return rtn;
}

void PackageInventoryProvider::SetCatalogDataset(
    const std::vector<PmProductDiscoveryRules>& discoveryRules )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_catalogRules = discoveryRules;
}

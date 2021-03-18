#include "PackageInventoryProvider.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IFileUtil.h"
#include "ISslUtil.h"
#include "PmLogger.h"

PackageInventoryProvider::PackageInventoryProvider( IFileUtil& fileUtil, ISslUtil& sslUtil ) :
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

    std::vector<PmProductDiscoveryRules> catalogRules;

    if( m_dependencies->ComponentManager().GetInstalledPackages( m_catalogDataset, catalogRules, packagesDiscovered ) == 0 ) {
        for( auto &package : packagesDiscovered.packages ) {
            for( auto it = package.configs.begin(); it != package.configs.end();) {
                std::string resolvedPath = m_dependencies->ComponentManager().ResolvePath( it->path );
                if( m_fileUtil.FileExists( resolvedPath ) ) {
                    auto sha256 = m_sslUtil.CalculateSHA256( resolvedPath );
                    it->sha256 = sha256.value();
                    it++;
                }
                else {
                    LOG_DEBUG( "Drop missing config %s", it->path.c_str() );
                    it = package.configs.erase( it );
                }
            }
        }
        inventory = packagesDiscovered;
        rtn = true;
    }

    return rtn;
}

void PackageInventoryProvider::SetCatalogDataset( 
    const std::vector<PmDiscoveryComponent>& discoveryList,
    const std::vector<PmProductDiscoveryRules>& discoveryRules )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_catalogDataset = discoveryList;
    m_catalogRules = discoveryRules;
}
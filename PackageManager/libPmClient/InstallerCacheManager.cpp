#include "InstallerCacheManager.h"
#include "PmTypes.h"
#include "IFileSysUtil.h"
#include "ISslUtil.h"
#include "IPmCloud.h"
#include "PmLogger.h"
#include "PackageException.h"
#include "PmConstants.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include <sstream>

InstallerCacheManager::InstallerCacheManager( IPmCloud& pmCloud, IFileSysUtil& fileUtil,  ISslUtil& sslUtil ) :
    m_pmCloud( pmCloud )
    , m_fileUtil( fileUtil )
    , m_sslUtil( sslUtil )
    , m_componentMgr( nullptr )
    , m_downloadPath( m_fileUtil.GetTempDir().append( "Cisco-UC" ) )
{

}

InstallerCacheManager::~InstallerCacheManager()
{

}

void InstallerCacheManager::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_componentMgr = &dep->ComponentManager();
}

std::string InstallerCacheManager::DownloadOrUpdateInstaller( const PmComponent& componentPackage )
{
    bool installerValid = false;
    std::stringstream ssError;

    std::filesystem::path installerPath = m_downloadPath;
    installerPath /= componentPackage.installerHash.empty() ?
        SanitizeComponentProductAndVersion( componentPackage.productAndVersion ) : 
        componentPackage.installerHash;
    installerPath += "." + componentPackage.installerType;
    installerPath.make_preferred();

    if ( componentPackage.installerHash.empty() ) {
        // An empty installerHash means the package is a QA deployed package ( using force_downstream_uri )
        LOG_ERROR( "Deleting QA package %s, ", installerPath.c_str() );
        DeleteInstaller( installerPath.generic_string() );
    }
    else if ( !ValidateInstaller( componentPackage, installerPath.generic_string() ) ) {
        DeleteInstaller( installerPath.generic_string() );
    }
    else {
        installerValid = true;
    }

    if ( !installerValid ) {
        if ( int httpResult = m_pmCloud.DownloadFile( componentPackage.installerUrl, installerPath.generic_string() ) != 200 ) {
            ssError << "Failed to download " << componentPackage.installerUrl << " to \"" << installerPath.generic_string() << "\". HTTP result: " << httpResult;
            throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_DOWNLOAD );
        }
        else {
            if ( !componentPackage.installerHash.empty() && !ValidateInstaller( componentPackage, installerPath.generic_string() ) ) {
                DeleteInstaller( installerPath.generic_string() );
                throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_HASH_MISMATCH );
            }
        }
    }

    return installerPath.generic_string();
}

void InstallerCacheManager::DeleteInstaller( const std::string& installerPath )
{
    if ( installerPath.empty() || !m_fileUtil.FileExists( installerPath ) ) {
        return;
    }

    LOG_DEBUG( "Removing %s", installerPath.c_str() );
    if ( m_fileUtil.DeleteFile( installerPath ) != 0 ) {
        LOG_ERROR( __FUNCTION__ ": Failed to remove %s", installerPath.c_str() );
    }
}

bool InstallerCacheManager::ValidateInstaller( const PmComponent& componentPackage, const std::string& installerPath )
{
    bool installerValid = false;

    if ( m_fileUtil.FileExists( installerPath ) ) {
        auto sha = m_sslUtil.CalculateSHA256( installerPath );

        if ( sha.has_value() && sha.value() == componentPackage.installerHash ) {
            installerValid = true;
        }
        else {
            LOG_ERROR( "Sha mismatch for %s. Actual %s Expected %d", installerPath.c_str(),
                sha.has_value() ? sha.value().c_str() : "Sha Failed",
                componentPackage.installerHash.c_str() );
        }
    }

    return installerValid;
}

std::string InstallerCacheManager::SanitizeComponentProductAndVersion( const std::string& productAndVersion )
{
    std::string sanitzedStr = productAndVersion;
    std::replace( sanitzedStr.begin(), sanitzedStr.end(), '/', '-' );
    std::replace( sanitzedStr.begin(), sanitzedStr.end(), '\\', '-' );

    return sanitzedStr;
}

void InstallerCacheManager::PruneInstallers( uint32_t ageInSeconds )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if ( !m_componentMgr ) {
        LOG_ERROR( "m_componentMgr is not initialied" );
        return;
    }

    std::vector<std::filesystem::path> results;
    std::filesystem::path searchPath = m_downloadPath / "*";
    searchPath.make_preferred();

    LOG_DEBUG( "Searching for Installers in %s", searchPath.generic_string().c_str() );
    if ( m_componentMgr->FileSearchWithWildCard( searchPath, results ) == 0 ) {
        time_t now = time( NULL );

        LOG_DEBUG( "Removing installers older than %d", now - ageInSeconds );

        for ( auto& file : results ) {
            std::string filename = file.make_preferred().generic_string();

            time_t lwt = m_fileUtil.LastWriteTime( filename );

            LOG_DEBUG( "Checking cache file: %s LastWrite %d", filename.c_str(), lwt );

            if( now - lwt > ageInSeconds ) {
                LOG_DEBUG( "Removing file from cache: %s", filename.c_str() );
                m_fileUtil.DeleteFile( filename );
            }
        }
    }
}
#include "ComponentPackageProcessor.h"
#include "IPmCloud.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include <filesystem>
#include "PmLogger.h"
#include <sstream>

ComponentPackageProcessor::ComponentPackageProcessor( IPmCloud& pmCloud ) :
    m_pmCloud( pmCloud )
    , m_dependencies( nullptr )
    , m_fileCount( 0 )
{
}

ComponentPackageProcessor::~ComponentPackageProcessor()
{
}

void ComponentPackageProcessor::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_dependencies = dep;
}

bool ComponentPackageProcessor::ProcessComponentPackage( PmComponent& componentPackage )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_dependencies ) {
        return false;
    }

    std::stringstream ss;
    //TODO: Should make this more random
    ss << std::filesystem::temp_directory_path().generic_string();
    ss << "PMInstaller_" << m_fileCount++ << "." << componentPackage.installerType;

    if( m_pmCloud.DownloadFile( componentPackage.installerUrl, ss.str() ) == 200 ) {
        componentPackage.installerPath = ss.str();

        //m_dependencies->ComponentManager().InstallComponent( componentPackage );

        LOG_DEBUG( "Removing %s", ss.str().c_str() );
        if( !std::filesystem::remove( std::filesystem::path( ss.str() ) ) ) {
            LOG_ERROR( "Failed to remove %s", ss.str().c_str() );
        }
    }

    return true;
}
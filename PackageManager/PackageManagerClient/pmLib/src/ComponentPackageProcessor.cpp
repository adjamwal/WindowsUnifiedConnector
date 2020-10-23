#include "ComponentPackageProcessor.h"
#include "IPmCloud.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IFileUtil.h"
#include "PmLogger.h"
#include <sstream>

ComponentPackageProcessor::ComponentPackageProcessor( IPmCloud& pmCloud, IFileUtil& fileUtil ) :
    m_pmCloud( pmCloud )
    , m_fileUtil( fileUtil )
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

    if( componentPackage.installerUrl.length() && componentPackage.installerType.length() ) {
        std::stringstream ss;
        //TODO: Should make this more random
        ss << m_fileUtil.GetTempDir() << "PMInstaller_" << m_fileCount++ << "." << componentPackage.installerType;

        if( m_pmCloud.DownloadFile( componentPackage.installerUrl, ss.str() ) == 200 ) {
            componentPackage.installerPath = ss.str();

            //m_dependencies->ComponentManager().InstallComponent( componentPackage );

            LOG_DEBUG( "Removing %s", ss.str().c_str() );
            if( m_fileUtil.DeleteFile( ss.str() ) != 0 ) {
                LOG_ERROR( "Failed to remove %s", ss.str().c_str() );
            }
        }
    }

    for each( auto config in componentPackage.configs ) {
        std::stringstream ss;
        ss << m_fileUtil.GetTempDir() << "PMConfig_" << m_fileCount++;

        //base64 decode

        //m_dependencies->ComponentManager().DeployConfiguration( PmPackageConfigration );

        LOG_DEBUG( "Removing %s", ss.str().c_str() );
        if( m_fileUtil.DeleteFile( ss.str() ) != 0 ) {
            LOG_ERROR( "Failed to remove %s", ss.str().c_str() );
        }
    }

    return true;
}
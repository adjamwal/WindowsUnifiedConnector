#include "ComponentPackageProcessor.h"
#include "IPmCloud.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IFileUtil.h"
#include "ISslUtil.h"
#include "IPackageConfigProcessor.h"
#include "PmLogger.h"
#include <sstream>

ComponentPackageProcessor::ComponentPackageProcessor( IPmCloud& pmCloud, 
    IFileUtil& fileUtil, 
    ISslUtil& sslUtil,
    IPackageConfigProcessor& configProcessor ) :
    m_pmCloud( pmCloud )
    , m_fileUtil( fileUtil )
    , m_sslUtil( sslUtil )
    , m_configProcessor( configProcessor )
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
    m_configProcessor.Initialize( dep );
}

bool ComponentPackageProcessor::ProcessComponentPackage( PmComponent& componentPackage )
{
    bool rtn = false;
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

            // only validate hash if installerHash is not empty
            if ( !componentPackage.installerHash.empty() )
            {
                auto sha256 = m_sslUtil.CalculateSHA256( ss.str() );

                if ( sha256.has_value() )
                {
                    if ( sha256.value() == componentPackage.installerHash )
                    {
                        std::string errorText;
                        int32_t updated = m_dependencies->ComponentManager().UpdateComponent( componentPackage, errorText );

                        if ( updated != 0 )
                        {
                            LOG_ERROR( "Failed to Update Component: (%d) %s", updated, errorText.c_str() );
                            //TODO: Report error
                        }
                        else {
                            rtn = ProcessComponentPackageConfigs( componentPackage );
                        }
                    }
                    else
                    {
                        LOG_ERROR( "Failed to match hash of download. Calculate Hash: %s Cloud Hash: %s", sha256, componentPackage.installerHash );
                        //TODO: Report error
                    }
                }
                else
                {
                    LOG_ERROR( "Failed to calculate sha256 of %s", ss.str().c_str() );
                    //TODO: Report error
                }
            }
            else
            {
                std::string errorText;
                int32_t updated = m_dependencies->ComponentManager().UpdateComponent( componentPackage, errorText );

                if ( updated != 0 )
                {
                    LOG_ERROR( "Failed to Update Component: (%d) %s", updated, errorText.c_str() );
                    //TODO: Report error
                }
                else {
                    rtn = ProcessComponentPackageConfigs( componentPackage );
                }
            }
            
            LOG_DEBUG( "Removing %s", ss.str().c_str() );
            if( m_fileUtil.DeleteFile( ss.str() ) != 0 ) {
                LOG_ERROR( "Failed to remove %s", ss.str().c_str() );
            }
        }
    }
    else {
        rtn = ProcessComponentPackageConfigs( componentPackage );
    }


    return rtn;
}

bool ComponentPackageProcessor::ProcessComponentPackageConfigs( PmComponent& componentPackage )
{
    bool rtn = true;

    for( auto config : componentPackage.configs ) {
        rtn = m_configProcessor.ProcessConfig( config );
    }
    
    return rtn;
}

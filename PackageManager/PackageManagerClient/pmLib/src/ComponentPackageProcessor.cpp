#include "ComponentPackageProcessor.h"
#include "IPmCloud.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IFileUtil.h"
#include "ISslUtil.h"
#include "PmLogger.h"
#include <sstream>

ComponentPackageProcessor::ComponentPackageProcessor( IPmCloud& pmCloud, 
    IFileUtil& fileUtil, 
    ISslUtil& sslUtil ) :
    m_pmCloud( pmCloud )
    , m_fileUtil( fileUtil )
    , m_sslUtil( sslUtil )
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

            std::string errorText;
            int32_t updated = m_dependencies->ComponentManager().UpdateComponent( componentPackage, errorText );

            if( updated != 0 )
            {
                LOG_ERROR( "Failed to Update Component: (%d) %s", updated, errorText.c_str() );
                //TODO: Report error
            }
            else {
                rtn = ProcessComponentPackageConfigs( componentPackage );
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
    bool rtn = false;

    if( !componentPackage.installLocation.empty() ) {
        for each( auto config in componentPackage.configs ) {
            rtn = ProcessComponentConfig( config );
        }
    }
    else {
        rtn = true;
    }


    return rtn;
}

bool ComponentPackageProcessor::ProcessComponentConfig( PackageConfigInfo& config )
{
    bool rtn = false;
    std::vector<uint8_t> configData;
    if( config.installLocation.empty() ) {
        LOG_ERROR( "No install path" );
    }
    else if( m_sslUtil.DecodeBase64( config.contents, configData ) != 0 ) {
        LOG_ERROR( "Failed to decode %s", config.contents );
    }
    else {
        std::stringstream ss;
        FileUtilHandle* handle = NULL;
        ss << m_fileUtil.GetTempDir() << "PMConfig_" << m_fileCount++;

        if( ( handle = m_fileUtil.PmCreateFile( ss.str() ) ) == NULL ) {
            LOG_ERROR( "Failed to create %s", ss.str().c_str() );
        }
        else if( m_fileUtil.AppendFile( handle, configData.data(), configData.size() ) != 0 ) {
            LOG_ERROR( "Failed to write to %s", ss.str().c_str() );
            m_fileUtil.CloseFile( handle );

            LOG_DEBUG( "Removing %s", ss.str().c_str() );
            if( m_fileUtil.DeleteFile( ss.str() ) != 0 ) {
                LOG_ERROR( "Failed to remove %s", ss.str().c_str() );
            }
        }
        else {
            m_fileUtil.CloseFile( handle );
            config.verifyPath = ss.str();

            bool moveFile = true;
            if( !config.verifyBinPath.empty() ) {
                moveFile = m_dependencies->ComponentManager().DeployConfiguration( config ) == 0;
            }

            if( moveFile ) {
                if( m_fileUtil.Rename( ss.str(), config.installLocation, config.path ) == 0 ) {
                    rtn = true;
                }
            }
        }
    }

    return rtn;
}
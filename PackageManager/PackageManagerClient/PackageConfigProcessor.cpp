#include "PackageConfigProcessor.h"
#include "IFileUtil.h"
#include "ISslUtil.h"
#include "PmTypes.h"
#include "PmLogger.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include <sstream>

PackageConfigProcessor::PackageConfigProcessor( IFileUtil& fileUtil, ISslUtil& sslUtil ) :
    m_fileUtil( fileUtil )
    , m_sslUtil( sslUtil )
    , m_dependencies( nullptr )
    , m_fileCount( 0 )
{

}

PackageConfigProcessor::~PackageConfigProcessor()
{

}

void PackageConfigProcessor::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_dependencies = dep;
}

bool PackageConfigProcessor::ProcessConfig( PackageConfigInfo& config )
{
    if( config.deleteConfig ) {
        return RemoveConfig( config );
    }
    else {
        return AddConfig( config );
    }
}

bool PackageConfigProcessor::AddConfig( PackageConfigInfo& config )
{
    bool rtn = false;
    std::vector<uint8_t> configData;

    if( m_sslUtil.DecodeBase64( config.contents, configData ) != 0 ) {
        LOG_ERROR( "Failed to decode %s", config.contents );
    }
    else {
        std::stringstream ss;
        FileUtilHandle* handle = NULL;
        ss << m_fileUtil.GetTempDir() << "PMConfig_" << m_fileCount++;

        if( ( handle = m_fileUtil.PmCreateFile( ss.str() ) ) == NULL ) {
            LOG_ERROR( "Failed to create %s", ss.str().c_str() );
        }
        else if( m_fileUtil.AppendFile( handle, configData.data(), configData.size() ) == 0 ) {
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

            // only validate hash if installerHash is not empty
            if( !config.sha256.empty() )
            {
                auto sha256 = m_sslUtil.CalculateSHA256( config.verifyPath );
                if( sha256 != config.sha256 ) {
                    moveFile = false;
                }
            }

            if( moveFile && !config.verifyBinPath.empty() ) {
                moveFile = m_dependencies->ComponentManager().DeployConfiguration( config ) == 0;
            }

            if( moveFile ) {
                std::string location = m_dependencies->ComponentManager().ResolvePath( config.installLocation, config.path );

                if( m_fileUtil.Rename( ss.str(), location ) == 0 ) {
                    rtn = true;
                }
                else {
                    LOG_ERROR( "Rename Failed. Removing %s", ss.str().c_str() );
                    if( m_fileUtil.DeleteFile( ss.str() ) != 0 ) {
                        LOG_ERROR( "Failed to remove %s", ss.str().c_str() );
                    }
                }
            }
        }
    }

    return rtn;
}

bool PackageConfigProcessor::RemoveConfig( PackageConfigInfo& config )
{
    bool rtn = false;

    std::string location = m_dependencies->ComponentManager().ResolvePath( config.installLocation, config.path );

    if( !location.empty() ) {
        if( m_fileUtil.DeleteFile( location ) != 0 ) {
            LOG_ERROR( "Failed to remove %s", location.c_str() );
        }
        else {
            LOG_DEBUG( "Remove config file %s", location.c_str() );
            rtn = true;
        }
    }
    else {
        LOG_ERROR( "Failed to resolve file %s", config.path.c_str() );
    }

    return rtn;
}
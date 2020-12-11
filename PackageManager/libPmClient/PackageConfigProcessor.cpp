#include "PackageConfigProcessor.h"
#include "IFileUtil.h"
#include "ISslUtil.h"
#include "IUcidAdapter.h"
#include "CloudEventBuilder.h"
#include "CloudEventPublisher.h"
#include "PmTypes.h"
#include "PmLogger.h"
#include "PmConstants.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include <sstream>
#include "RandomUtil.h"

PackageConfigProcessor::PackageConfigProcessor(
    IFileUtil& fileUtil,
    ISslUtil& sslUtil,
    IUcidAdapter& ucidAdapter,
    ICloudEventBuilder& eventBuilder,
    ICloudEventPublisher& eventPublisher )
    : m_fileUtil( fileUtil )
    , m_sslUtil( sslUtil )
    , m_ucidAdapter( ucidAdapter )
    , m_eventBuilder( eventBuilder )
    , m_eventPublisher( eventPublisher )
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
    m_eventPublisher.SetToken( m_ucidAdapter.GetAccessToken() );
    m_eventBuilder.Reset();
    m_eventBuilder.WithUCID( m_ucidAdapter.GetIdentity() );
    m_eventBuilder.WithPackageID( config.forComponentID );

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
    std::string targetLocation = m_fileUtil.AppendPath( config.installLocation, config.path );

    m_eventBuilder.WithType( CloudEventType::pkgreconfig );
    m_eventBuilder.WithNewFile( config.path, config.sha256, 0 );
    if( m_fileUtil.FileExists( targetLocation ) )
    {
        auto old_sha256 = m_sslUtil.CalculateSHA256( targetLocation );
        m_eventBuilder.WithOldFile( config.path, old_sha256.value(), m_fileUtil.FileSize( targetLocation ) );
    }

    if( m_sslUtil.DecodeBase64( config.contents, configData ) != 0 ) {
        std::stringstream ssError;
        ssError << "Failed to decode " << config.contents;
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_CONFIG_DECODE, ssError.str() );
        LOG_ERROR( "%s", ssError.str().c_str() );
        m_eventPublisher.Publish( m_eventBuilder );

        return rtn;
    }

    std::stringstream ss;
    FileUtilHandle* handle = NULL;

    ss << m_fileUtil.GetTempDir() << "tmpPmConf_" << m_fileCount++ << RandomUtil::GetString( 10 );

    if( ( handle = m_fileUtil.PmCreateFile( ss.str() ) ) == NULL )
    {
        std::stringstream ssError;
        ssError << "Failed to create " << ss.str();
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_CONFIG_CREATE, ssError.str() );
        LOG_ERROR( "%s", ssError.str().c_str() );
        m_eventPublisher.Publish( m_eventBuilder );

        return rtn;
    }

    if( m_fileUtil.AppendFile( handle, configData.data(), configData.size() ) == 0 )
    {
        m_fileUtil.CloseFile( handle );

        std::stringstream ssError;
        ssError << "Failed to write to " << ss.str();
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_CONFIG_WRITE, ssError.str() );
        LOG_ERROR( "%s", ssError.str().c_str() );
        m_eventPublisher.Publish( m_eventBuilder );

        LOG_DEBUG( "Removing %s", ss.str().c_str() );
        if( m_fileUtil.DeleteFile( ss.str() ) != 0 ) {
            LOG_ERROR( "Failed to remove %s", ss.str().c_str() );
        }

        return rtn;
    }

    m_fileUtil.CloseFile( handle );
    config.verifyPath = ss.str();

    auto sha256 = m_sslUtil.CalculateSHA256( ss.str() );
    m_eventBuilder.WithNewFile( 
        config.path, 
        sha256.has_value() ? sha256.value() : config.sha256,
        m_fileUtil.FileSize( ss.str() ) 
    );

    bool moveFile = true;

    // only validate hash if installerHash is not empty
    if( !config.sha256.empty() )
    {
        if( sha256 != config.sha256 ) {
            moveFile = false;
        }
    }

    if( moveFile && !config.verifyBinPath.empty() ) {
        moveFile = m_dependencies->ComponentManager().DeployConfiguration( config ) == 0;
    }

    if( moveFile ) {
        if( m_fileUtil.Rename( ss.str(), targetLocation ) == 0 ) {
            rtn = true;
        }
        else {
            LOG_ERROR( "Rename Failed. Removing %s", ss.str().c_str() );
            if( m_fileUtil.DeleteFile( ss.str() ) != 0 ) {
                LOG_ERROR( "Failed to remove %s", ss.str().c_str() );
            }
        }
    }

    if( !rtn )
    {
        std::stringstream ssError;
        ssError << "Failed to deploy configuration to " << targetLocation;
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_CONFIG_DEPLOY, ssError.str() );
    }

    m_eventPublisher.Publish( m_eventBuilder );

    return rtn;
}

bool PackageConfigProcessor::RemoveConfig( PackageConfigInfo& config )
{
    bool rtn = false;

    std::string targetLocation = m_fileUtil.AppendPath( config.installLocation, config.path );

    m_eventBuilder.WithType( CloudEventType::pkgreconfig );
    auto sha256 = m_sslUtil.CalculateSHA256( targetLocation );
    m_eventBuilder.WithOldFile( 
        config.path, 
        sha256.has_value() ? sha256.value() : config.sha256,
        m_fileUtil.FileSize( targetLocation ) );

    if( !targetLocation.empty() ) {
        if( m_fileUtil.DeleteFile( targetLocation ) != 0 )
        {
            std::stringstream ssError;
            ssError << "Failed to remove " << targetLocation;
            m_eventBuilder.WithError( UCPM_EVENT_ERROR_CONFIG_REMOVE, ssError.str() );
            LOG_ERROR( "%s", ssError.str().c_str() );
        }
        else {
            LOG_DEBUG( "Removed config file %s", targetLocation.c_str() );
            rtn = true;
        }
    }
    else
    {
        std::stringstream ssError;
        ssError << "Failed to resolve file " << targetLocation;
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_CONFIG_RESOLVE, ssError.str() );
        LOG_ERROR( "%s", ssError.str().c_str() );
    }

    m_eventPublisher.Publish( m_eventBuilder );

    return rtn;
}
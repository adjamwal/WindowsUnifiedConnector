#include "ComponentPackageProcessor.h"
#include "IPmCloud.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IFileUtil.h"
#include "ISslUtil.h"
#include "IPackageConfigProcessor.h"
#include "IUcidAdapter.h"
#include "ICloudEventBuilder.h"
#include "ICloudEventPublisher.h"
#include "IUcUpgradeEventHandler.h"
#include "PmLogger.h"
#include "PmConstants.h"
#include "PackageException.h"
#include "RandomUtil.h"
#include <sstream>
#include <iostream>
#include <vector>

ComponentPackageProcessor::ComponentPackageProcessor(
    IPmCloud& pmCloud,
    IFileUtil& fileUtil,
    ISslUtil& sslUtil,
    IPackageConfigProcessor& configProcessor,
    IUcidAdapter& ucidAdapter,
    ICloudEventBuilder& eventBuilder,
    ICloudEventPublisher& eventPublisher,
    IUcUpgradeEventHandler& ucUpgradeEventHandler )
    : m_pmCloud( pmCloud )
    , m_fileUtil( fileUtil )
    , m_sslUtil( sslUtil )
    , m_configProcessor( configProcessor )
    , m_ucidAdapter( ucidAdapter )
    , m_eventBuilder( eventBuilder )
    , m_eventPublisher( eventPublisher )
    , m_ucUpgradeEventHandler( ucUpgradeEventHandler )
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
    m_ucUpgradeEventHandler.Initialize( dep );
}

bool ComponentPackageProcessor::IsActionable( PmComponent& componentPackage )
{
    return ( componentPackage.installerUrl.length() > 0 && componentPackage.installerType.length() > 0 );
}

bool ComponentPackageProcessor::HasConfigs( PmComponent& componentPackage )
{
    return componentPackage.configs.size() > 0;
}

bool ComponentPackageProcessor::ProcessPackageBinaries( PmComponent& componentPackage )
{
    if( !IsActionable( componentPackage ) )
    {
        return false;
    }

    bool rtn = false;
    std::stringstream ssError;
    std::string tempPackageFile;
    std::optional<std::string> tempSha256;

    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_dependencies )
    {
        LOG_ERROR( __FUNCTION__ ": Dependencies not initialized" );
        return false;
    }

    m_eventPublisher.SetToken( m_ucidAdapter.GetAccessToken() );

    m_eventBuilder.Reset();
    m_eventBuilder.WithUCID( m_ucidAdapter.GetIdentity() );
    m_eventBuilder.WithPackageID( componentPackage.productAndVersion );

    bool isAlreadyInstalled = IsPackageFoundLocally( m_eventBuilder.GetPackageName(), m_eventBuilder.GetPackageVersion() );
    m_eventBuilder.WithType( isAlreadyInstalled ? CloudEventType::pkgreconfig : CloudEventType::pkginstall );
    m_eventBuilder.WithNewFile( componentPackage.installerUrl, componentPackage.installerHash, 0 ); //new file info gets updated after download

    try
    {
        DownloadAsTempFile( componentPackage, tempPackageFile );
        componentPackage.installerPath = tempPackageFile;

        tempSha256 = m_sslUtil.CalculateSHA256( tempPackageFile );
        if( !tempSha256.has_value() )
        {
            ssError << "Failed to calculate sha256 of " << tempPackageFile;
            throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_HASH_CALC );
        }

        m_eventBuilder.WithNewFile(
            componentPackage.installerUrl,
            tempSha256.has_value() ? tempSha256.value() : componentPackage.installerHash,
            tempPackageFile.empty() ? 0 : m_fileUtil.FileSize( tempPackageFile ) );

        // only validate hash if installerHash is not empty
        if( !componentPackage.installerHash.empty() &&
            tempSha256.value() != componentPackage.installerHash )
        {
            ssError << "Failed to match hash of download. Calculated Hash: " << tempSha256.value() << ", Cloud Hash: " << componentPackage.installerHash;
            throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_HASH_MISMATCH );
        }

        m_ucUpgradeEventHandler.StoreUcUpgradeEvent( m_eventBuilder.Build() );
        std::string updErrText;
        int32_t updErrCode = m_dependencies->ComponentManager().UpdateComponent( componentPackage, updErrText );

        if( updErrCode != 0 )
        {
            ssError << "Failed to Validate Component Hash. Error " << updErrCode << ": " << updErrText;
            throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_UPDATE );
        }

        rtn = true;
    }
    catch( PackageException& ex )
    {
        m_eventBuilder.WithError( ex.whatCode(), ex.what() );
        LOG_ERROR( __FUNCTION__ ": %s", ex.what() );
    }
    catch( std::exception& ex )
    {
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_UNDEFINED_EXCEPTION, ex.what() );
        LOG_ERROR( __FUNCTION__ ": %s", ex.what() );
    }
    catch( ... )
    {
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_UNDEFINED_EXCEPTION, "Unknown processing exception" );
        LOG_ERROR( __FUNCTION__ ": Unknown processing exception" );
    }

    m_eventPublisher.Publish( m_eventBuilder );

    CleanupTempDownload( tempPackageFile );

    return rtn;
}

bool ComponentPackageProcessor::IsPackageFoundLocally( const std::string& name, const std::string& version )
{
    if( !m_dependencies ) return false;

    PackageInventory inventory;
    m_dependencies->ComponentManager().GetCachedInventory( inventory );

    for( auto item : inventory.packages )
    {
        if( item.product == name &&
            ( version.empty() || version == item.version )
            ) return true;
    }

    return false;
}

void ComponentPackageProcessor::DownloadAsTempFile( const PmComponent& componentPackage, std::string& tempPackageFile )
{
    std::stringstream ss;
    std::stringstream ssError;

    ss << m_fileUtil.GetTempDir() << "tmpPmInst_" << m_fileCount++ << RandomUtil::GetString(10) << "." << componentPackage.installerType;

    if( int httpResult = m_pmCloud.DownloadFile( componentPackage.installerUrl, ss.str() ) != 200 )
    {
        ssError << "Failed to download " << componentPackage.installerUrl << " to \"" << ss.str() << "\". HTTP result: " << httpResult;
        throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_DOWNLOAD );
    }

    tempPackageFile = ss.str();
}

void ComponentPackageProcessor::CleanupTempDownload( std::string tempFilePath )
{
    if( tempFilePath.empty() || !m_fileUtil.FileExists( tempFilePath ) )
    {
        return;
    }

    LOG_DEBUG( "Removing %s", tempFilePath.c_str() );
    if( m_fileUtil.DeleteFile( tempFilePath ) != 0 )
    {
        LOG_ERROR( __FUNCTION__ ": Failed to remove %s", tempFilePath.c_str() );
    }
}

bool ComponentPackageProcessor::ProcessConfigsForPackage( PmComponent& componentPackage )
{
    int failedConfigs = 0;
    for( auto config : componentPackage.configs )
    {
        bool processed = false;
        
        try
        {
            LOG_DEBUG( __FUNCTION__ ": Process config %s, for package %s", config.path.c_str(), componentPackage.productAndVersion.c_str() );
            config.forProductAndVersion = componentPackage.productAndVersion;
            processed = m_configProcessor.ProcessConfig( config );
        }
        catch( ... )
        {
            LOG_ERROR( __FUNCTION__ ": Failed to process %s", config.path.c_str() );
        }

        failedConfigs += processed ? 0 : 1;
    }

    if( failedConfigs > 0 )
    {
        LOG_ERROR( __FUNCTION__ ": Failed to process %d configs for package %s", failedConfigs, componentPackage.productAndVersion.c_str() );
    }

    return failedConfigs == 0;
}

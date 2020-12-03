#include "ComponentPackageProcessor.h"
#include "IPmCloud.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IFileUtil.h"
#include "ISslUtil.h"
#include "IPackageConfigProcessor.h"
#include "IUcidAdapter.h"
#include "CloudEventBuilder.h"
#include "CloudEventPublisher.h"
#include "PmLogger.h"
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
    ICloudEventPublisher& eventPublisher )
    : m_pmCloud( pmCloud )
    , m_fileUtil( fileUtil )
    , m_sslUtil( sslUtil )
    , m_configProcessor( configProcessor )
    , m_ucidAdapter( ucidAdapter )
    , m_eventBuilder( eventBuilder )
    , m_eventPublisher( eventPublisher )
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

    if( !m_dependencies )
    {
        LOG_ERROR( "Dependencies not initialized" );
        return false;
    }

    if( componentPackage.installerUrl.length() == 0 || componentPackage.installerType.length() == 0 )
    {
        return ProcessComponentPackageConfigs( componentPackage );
    }

    m_eventPublisher.SetToken( m_ucidAdapter.GetAccessToken() );

    m_eventBuilder.Reset();
    m_eventBuilder.WithUCID( m_ucidAdapter.GetIdentity() );
    m_eventBuilder.WithPackageID( componentPackage.packageNameAndVersion );

    bool isAlreadyInstalled = IsPackageFoundLocally( componentPackage.packageNameAndVersion, m_eventBuilder.GetPackageName() );
    m_eventBuilder.WithType( isAlreadyInstalled ? CloudEventType::pkgreconfig : CloudEventType::pkginstall );

    std::stringstream ss;
    //TODO: Should make this more random
    ss << m_fileUtil.GetTempDir() << "PMInstaller_" << m_fileCount++ << "." << componentPackage.installerType;

    int dlResult = m_pmCloud.DownloadFile( componentPackage.installerUrl, ss.str() );
    if( dlResult != 200 )
    {
        m_eventBuilder.WithNewFile( componentPackage.installerUrl, componentPackage.installerHash, 0 );

        std::stringstream ssError;
        ssError << "Download failed. Result code: " << dlResult << ", uri: " << componentPackage.installerUrl;
        m_eventBuilder.WithError( -1, ssError.str() );

        LOG_ERROR( "%s", ssError.str().c_str() );
        m_eventPublisher.Publish( m_eventBuilder );

        return false;
    }

    componentPackage.installerPath = ss.str();
    auto sha256 = m_sslUtil.CalculateSHA256( ss.str() );
    m_eventBuilder.WithNewFile( componentPackage.installerUrl, sha256.value(), m_fileUtil.FileSize( componentPackage.installerPath ) );

    // only validate hash if installerHash is not empty
    if( !componentPackage.installerHash.empty() )
    {
        if( sha256.has_value() )
        {
            if( sha256.value() == componentPackage.installerHash )
            {
                std::string errorText;
                int32_t updated = m_dependencies->ComponentManager().UpdateComponent( componentPackage, errorText );

                if( updated != 0 )
                {
                    std::stringstream ssError;
                    ssError << "Failed to Update Component. Error " << updated << errorText;
                    m_eventBuilder.WithError( -1, ssError.str() );

                    LOG_ERROR( "%s", ssError.str().c_str() );
                    m_eventPublisher.Publish( m_eventBuilder );
                }
                else {
                    m_eventPublisher.Publish( m_eventBuilder );
                    rtn = ProcessComponentPackageConfigs( componentPackage );
                }
            }
            else
            {
                std::stringstream ssError;
                ssError << "Failed to match hash of download. Calculated Hash: " << sha256.value() << ", Cloud Hash: " << componentPackage.installerHash;
                m_eventBuilder.WithError( -1, ssError.str() );

                LOG_ERROR( "%s", ssError.str().c_str() );
                m_eventPublisher.Publish( m_eventBuilder );
            }
        }
        else
        {
            std::stringstream ssError;
            ssError << "Failed to calculate sha256 of " << ss.str();
            m_eventBuilder.WithError( -1, ssError.str() );

            LOG_ERROR( "%s", ssError.str().c_str() );
            m_eventPublisher.Publish( m_eventBuilder );
        }
    }
    else
    {
        std::string errorText;
        int32_t updated = m_dependencies->ComponentManager().UpdateComponent( componentPackage, errorText );

        if( updated != 0 )
        {
            std::stringstream ssError;
            ssError << "Failed to Update Component. Error " << updated << errorText;
            m_eventBuilder.WithError( -1, ssError.str() );

            LOG_ERROR( "%s", ssError.str().c_str() );
            m_eventPublisher.Publish( m_eventBuilder );
        }
        else {
            m_eventPublisher.Publish( m_eventBuilder );
            rtn = ProcessComponentPackageConfigs( componentPackage );
        }
    }

    LOG_DEBUG( "Removing %s", ss.str().c_str() );
    if( m_fileUtil.DeleteFile( ss.str() ) != 0 ) {
        LOG_ERROR( "Failed to remove %s", ss.str().c_str() );
    }

    return rtn;
}

bool ComponentPackageProcessor::IsPackageFoundLocally( const std::string& nameAndVersion, const std::string& nameOnly )
{
    if( !m_dependencies ) return false;

    PmDiscoveryComponent searchTarget = { nameAndVersion, nameOnly };
    std::vector<PmDiscoveryComponent> searchFor( { searchTarget } );
    PackageInventory detectedInventory = {};

    m_dependencies->ComponentManager().GetInstalledPackages( searchFor, detectedInventory );

    return detectedInventory.packages.size() > 0;
}

bool ComponentPackageProcessor::ProcessComponentPackageConfigs( PmComponent& componentPackage )
{
    bool rtn = true;

    for( auto config : componentPackage.configs ) {
        config.forComponentID = componentPackage.packageNameAndVersion;
        rtn = m_configProcessor.ProcessConfig( config );
    }

    return rtn;
}

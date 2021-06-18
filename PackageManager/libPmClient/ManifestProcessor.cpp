#include "ManifestProcessor.h"
#include "PmTypes.h"
#include "PmLogger.h"
#include <iostream>
#include <sstream>

ManifestProcessor::ManifestProcessor(
    IPmManifest& manifest,
    IComponentPackageProcessor& componentProcessor )
    : m_manifest( manifest )
    , m_componentProcessor( componentProcessor )
{
}

ManifestProcessor::~ManifestProcessor()
{
}

void ManifestProcessor::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_manifest.Initialize( dep );
    m_componentProcessor.Initialize( dep );
}

bool ManifestProcessor::ProcessManifest( std::string checkinManifest, bool& isRebootRequired )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    isRebootRequired = false;

    if( m_manifest.ParseManifest( checkinManifest ) != 0 )
    {
        throw std::exception( __FUNCTION__": Failed to process manifest" );
    }

    auto packages = m_manifest.GetPackageList();

    PreDownloadAllFromManifest( packages );
    ProcessDownloadedPackagesAndConfigs( packages, isRebootRequired );

    return true;
}

void ManifestProcessor::PreDownloadAllFromManifest( std::vector<PmComponent>& packages )
{
    for( auto& package : packages )
    {
        try
        {
            m_componentProcessor.DownloadPackageBinary( package );
            LOG_DEBUG( __FUNCTION__ ": Downloaded: %s", package.downloadedInstallerPath.c_str() );
        }
        catch( ... )
        {
            LOG_ERROR( __FUNCTION__ ": Failed to download binary for package: %s", package.productAndVersion.c_str() );
        }
    }
}

void ManifestProcessor::ProcessDownloadedPackagesAndConfigs( std::vector<PmComponent>& packages, bool& isRebootRequired )
{
    int failedPackages = 0;
    isRebootRequired = false;

    for( auto& package : packages )
    {
        bool processed = false;

        try
        {
            processed =
                ( m_componentProcessor.ProcessPackageBinary( package ) ) &&
                ( !m_componentProcessor.HasConfigs( package ) || m_componentProcessor.ProcessConfigsForPackage( package ) );

            isRebootRequired |= package.postInstallRebootRequired;

            LOG_DEBUG( __FUNCTION__ ": Processed=%d: %s, postInstallRebootRequired=%d", 
                processed, package.productAndVersion.c_str(), package.postInstallRebootRequired );
        }
        catch( ... ) {
            LOG_ERROR( __FUNCTION__ ": Failed to process package: %s", package.productAndVersion.c_str() );
        }

        failedPackages += processed ? 0 : 1;
    }

    if( failedPackages > 0 )
    {
        std::stringstream ss;
        ss << __FUNCTION__ << ": Failed to process " << failedPackages << " component package(s)";
        throw std::exception( ss.str().c_str() );
    }
}

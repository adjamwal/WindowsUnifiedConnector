#include "ManifestProcessor.h"
#include "PackageException.h"
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
        m_componentProcessor.DownloadPackageBinary( package );
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

            LOG_DEBUG( "Processed=%d: %s, postInstallRebootRequired=%d", 
                processed, package.productAndVersion.c_str(), package.postInstallRebootRequired );
        }
        catch( std::exception& e ) {
            LOG_ERROR( "Failed to process package: %s, %s", package.productAndVersion.c_str(), e.what() );
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

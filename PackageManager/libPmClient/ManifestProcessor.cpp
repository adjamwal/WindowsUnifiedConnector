#include "ManifestProcessor.h"
#include "PmTypes.h"
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

bool ManifestProcessor::ProcessManifest( std::string checkinManifest )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( m_manifest.ParseManifest( checkinManifest ) != 0 )
    {
        //PmSendEvent() bad manifest
        throw std::exception( __FUNCTION__": Failed to process manifest" );
    }

    int failedPackages = 0;
    for( auto package : m_manifest.GetPackageList() )
    {
        bool processed = 
            ( !m_componentProcessor.IsActionable( package ) || m_componentProcessor.ProcessPackageBinaries( package ) ) &&
            ( !m_componentProcessor.HasConfigs( package ) || m_componentProcessor.ProcessConfigsForPackage( package ) );

        failedPackages += processed ? 0 : 1;
    }

    if( failedPackages > 0 )
    {
        std::stringstream ss;
        ss << __FUNCTION__ << ": Failed to process " << failedPackages << " component package(s)";
        throw std::exception( ss.str().c_str() );
    }

    //PmSendEvent() success

    return true;
}

#include "ManifestProcessor.h"
#include "PmTypes.h"

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

    m_componentProcessor.Initialize( dep );
}

bool ManifestProcessor::ProcessManifest( std::string checkinManifest )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( m_manifest.ParseManifest( checkinManifest ) != 0 )
    {
        //PmSendEvent() bad manifest
        throw std::exception( __FUNCTION__": Failed to process manifest" );
        return false;
    }

    for( auto package : m_manifest.GetPackageList() )
    {
        if( !m_componentProcessor.ProcessComponentPackage( package ) )
        {
            //PmSendEvent() bad manifest
            throw std::exception( __FUNCTION__": Failed to process component package" );
        }
    }

    //PmSendEvent() success

    return true;
}

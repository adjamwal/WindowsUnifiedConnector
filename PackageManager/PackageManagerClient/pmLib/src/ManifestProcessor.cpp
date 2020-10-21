#include "ManifestProcessor.h"
#include "PmTypes.h"

ManifestProcessor::ManifestProcessor(
    IPmManifest& manifest,
    IComponentPackageProcessor componentProcessor )
    : m_manifest( manifest )
    , m_componentProcessor( componentProcessor )
{
}

ManifestProcessor::~ManifestProcessor()
{
}

bool ManifestProcessor::ProcessManifest( std::string checkinManifest )
{
    if( !m_manifest.ParseManifest( checkinManifest ) )
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
        else {
            //PmSendEvent() success
        }
    }

    return true;
}

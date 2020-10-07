#include "PmManifest.h"
#include "PmTypes.h"

PmManifest::PmManifest()
{

}

PmManifest::~PmManifest()
{

}

int32_t PmManifest::ParseManifest( const std::string& manifestJson )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return -1;
}

std::vector<PmComponent> PmManifest::GetPackageList()
{
    std::vector<PmComponent> componentList;
    std::lock_guard<std::mutex> lock( m_mutex );

    return componentList;
}
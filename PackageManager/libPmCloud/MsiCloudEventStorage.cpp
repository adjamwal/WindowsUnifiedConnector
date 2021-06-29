#include "pch.h"
#include "MsiCloudEventStorage.h"

MsiCloudEventStorage::MsiCloudEventStorage()
{
}

MsiCloudEventStorage::~MsiCloudEventStorage()
{
}

void MsiCloudEventStorage::Initialize( IPmPlatformDependencies* dep )
{
    
}

bool MsiCloudEventStorage::SaveEvent( ICloudEventBuilder& event )
{
    return true;
}

bool MsiCloudEventStorage::SaveEvent( const std::string& event )
{
    return true;
}

std::vector<std::string> MsiCloudEventStorage::ReadAndRemoveEvents()
{
    return std::vector<std::string>();
}

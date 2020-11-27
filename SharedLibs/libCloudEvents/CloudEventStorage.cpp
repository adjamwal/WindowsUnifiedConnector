#include "pch.h"
#include "CloudEventStorage.h"
#include <fstream>
#include <filesystem>

CloudEventStorage::CloudEventStorage( const std::string& fileName, IFileUtil& fileUtil ) :
    m_fileName( fileName ),
    m_fileUtil( fileUtil )
{
}

CloudEventStorage::~CloudEventStorage()
{
}

int32_t CloudEventStorage::SaveEvent( ICloudEventBuilder& event )
{
    auto eventStr = event.Build();

    return m_fileUtil.WriteLine( m_fileName, eventStr );
}

int32_t CloudEventStorage::SaveEvent( const std::string& event )
{
    return m_fileUtil.WriteLine( m_fileName, event );
}

std::vector<std::string> CloudEventStorage::ReadEvents()
{
    return m_fileUtil.ReadFileLines( m_fileName );
}

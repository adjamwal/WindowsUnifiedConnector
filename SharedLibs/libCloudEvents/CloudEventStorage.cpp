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

void CloudEventStorage::ReadEvents( std::vector<std::string>& events )
{
    events = m_fileUtil.ReadFileLines( m_fileName );
}

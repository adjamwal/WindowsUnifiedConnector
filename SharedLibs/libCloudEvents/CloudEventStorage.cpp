#include "pch.h"
#include "CloudEventStorage.h"
#include "WindowsUtilities.h"

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

    std::lock_guard<std::mutex> lock( m_mutex );

    return m_fileUtil.WriteLine( m_fileName, eventStr );
}

int32_t CloudEventStorage::SaveEvent( const std::string& event )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_fileUtil.WriteLine( m_fileName, event );
}

std::vector<std::string> CloudEventStorage::ReadEvents()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    auto ret = m_fileUtil.ReadFileLines( m_fileName );

    m_fileUtil.DeleteFile( m_fileName );

    return ret;
}

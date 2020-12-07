#include "pch.h"
#include "CloudEventStorage.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformConfiguration.h"

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

void CloudEventStorage::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_dependencies = dep;

    //Set full database path
    std::string filePath = m_dependencies->Configuration().GetDataDirectory();
    filePath.append( "\\" ).append( m_fileName );
    m_fullPath = filePath;
}

int32_t CloudEventStorage::SaveEvent( ICloudEventBuilder& event )
{
    if ( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    auto eventStr = event.Build();

    std::lock_guard<std::mutex> lock( m_mutex );

    return m_fileUtil.WriteLine( m_fullPath, eventStr );
}

int32_t CloudEventStorage::SaveEvent( const std::string& event )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if ( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    return m_fileUtil.WriteLine( m_fullPath, event );
}

std::vector<std::string> CloudEventStorage::ReadEvents()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if ( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    auto ret = m_fileUtil.ReadFileLines( m_fullPath );

    m_fileUtil.DeleteFile( m_fullPath );

    return ret;
}

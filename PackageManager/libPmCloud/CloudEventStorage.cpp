#include "pch.h"
#include "CloudEventStorage.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformConfiguration.h"
#include "PmLogger.h"
#include "TimeUtil.h"

#include <fstream>
#include <filesystem>

CloudEventStorage::CloudEventStorage( const std::string& fileName, IFileSysUtil& fileUtil, IPmConfig& pmConfig )
    : m_fileName( fileName )
    , m_fileUtil( fileUtil )
    , m_pmConfig( pmConfig )
    , m_dependencies( nullptr )
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
    std::string filePath = m_dependencies->Configuration().GetInstallDirectory();
    filePath.append( "\\" ).append( m_fileName );
    m_fullPath = filePath;

    LOG_DEBUG( __FUNCTION__ ": CloudEventStorage path: %s", m_fullPath.c_str() );
}

bool CloudEventStorage::SaveEvent( ICloudEventBuilder& event )
{
    if( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    auto eventStr = event.Build();

    __time64_t eventTimeMs = TimeUtil::RFC3339ToMilliTs( event.GetRFC3339Tse() );
    //add 0 hundreds of msec to match the rfc3339 format
    __time64_t eventTtl = eventTimeMs + m_pmConfig.GetMaxEventTtlS() * 10; 

    if( eventTtl > TimeUtil::Now_HundredMilliTimeStamp() )
    {
        LOG_DEBUG( __FUNCTION__ ": Event expired: %s", eventStr.c_str() );
        return false;
    }

    return SaveEvent( eventStr );
}

bool CloudEventStorage::SaveEvent( const std::string& event )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    return m_fileUtil.WriteLine( m_fullPath, event );
}

std::vector<std::string> CloudEventStorage::ReadAndRemoveEvents()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    auto ret = m_fileUtil.ReadFileLines( m_fullPath );

    m_fileUtil.EraseFile( m_fullPath );

    return ret;
}

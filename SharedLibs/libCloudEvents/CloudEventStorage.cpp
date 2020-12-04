#include "pch.h"
#include "CloudEventStorage.h"
#include "WindowsUtilities.h"

#include <fstream>
#include <filesystem>
#include <locale>
#include <codecvt>

CloudEventStorage::CloudEventStorage( const std::string& fileName, IFileUtil& fileUtil ) :
    m_fileUtil( fileUtil )
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string logFilePath = converter.to_bytes( WindowsUtilities::GetDataDir() );
    logFilePath.append( "\\" ).append( fileName );
    m_fileName = logFilePath;
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

    return m_fileUtil.ReadFileLines( m_fileName );
}

#include "PmCloud.h"

PmCloud::PmCloud()
{
}

PmCloud::~PmCloud()
{

}

void PmCloud::SetUri( const std::string& uri )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_uri = uri;
}

void PmCloud::SetToken( const std::string& token )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_token = token;
}

int32_t PmCloud::Checkin( const std::string& payload, std::string& response )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return -1;
}

int32_t PmCloud::SendEvent( const std::string& payload )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return -1;
}

int32_t PmCloud::DownloadFile( const std::string& uri, const std::string filename )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return -1;
}
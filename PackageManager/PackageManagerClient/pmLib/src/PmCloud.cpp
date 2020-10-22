#include "PmCloud.h"

PmCloud::PmCloud( IPmHttp& http )
    : m_http( http )
    , m_certs( { 0 } )
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

void PmCloud::SetCerts( const PmHttpCertList& certs )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_certs = certs;
}

int32_t PmCloud::Checkin( const std::string& payload, std::string& response )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_http.Init( NULL, NULL, "" );
    m_http.SetCerts( m_certs );
    m_http.SetToken( m_token );

    int32_t respStatus = 0;
    m_http.HttpPost( m_uri, (void*)payload.c_str(), payload.length() + 1, response, respStatus );

    return respStatus;
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
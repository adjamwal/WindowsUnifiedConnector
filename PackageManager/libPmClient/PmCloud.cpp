#include "PmCloud.h"

PmCloud::PmCloud( IPmHttp& http )
    : m_http( http )
    , m_certs( { 0 } )
    , m_shutdownFunc( [] { return false; } )
    , m_userAgent( "PakcageManager" )
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

void PmCloud::SetUserAgent( const std::string& userAgent )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_userAgent = userAgent;
}

void PmCloud::SetShutdownFunc( std::function<bool()> shutdownFunc )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_shutdownFunc = shutdownFunc;
}

int32_t PmCloud::Checkin( const std::string& payload, std::string& response )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_http.Init( _ProgressCallback, this, m_userAgent );
    m_http.SetCerts( m_certs );
    m_http.SetToken( m_token );

    int32_t respStatus = 0;
    m_http.HttpPost( m_uri, (void*)payload.c_str(), payload.length() + 1, response, respStatus );
    m_http.Deinit();
    return respStatus;
}

int32_t PmCloud::Post( const std::string& url, void* data, size_t dataSize, std::string& response, int32_t& httpReturn )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_http.Init( _ProgressCallback, this, m_userAgent );
    m_http.SetCerts( m_certs );
    m_http.SetToken( m_token );

    int32_t respStatus = 0;
    respStatus = m_http.HttpPost( url, data, dataSize, response, httpReturn );
    m_http.Deinit();

    return respStatus;
}

int32_t PmCloud::DownloadFile( const std::string& uri, const std::string filename )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_http.Init( _ProgressCallback, this, m_userAgent );
    m_http.SetCerts( m_certs );
    m_http.SetToken( m_token );

    int32_t respStatus = 0;
    m_http.HttpDownload( uri, filename, respStatus );
    m_http.Deinit();
    return respStatus;
}

int PmCloud::ProgressCallback( PM_TYPEOF_OFF_T dltotal, PM_TYPEOF_OFF_T dlnow, PM_TYPEOF_OFF_T ultotal, PM_TYPEOF_OFF_T ulnow )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_shutdownFunc() ? 0 : -1;
}

int PmCloud::_ProgressCallback( void* clientp, PM_TYPEOF_OFF_T dltotal, PM_TYPEOF_OFF_T dlnow, PM_TYPEOF_OFF_T ultotal, PM_TYPEOF_OFF_T ulnow )
{
    int rtn = 0;
    if( clientp ) {
        PmCloud* _this = ( PmCloud* )clientp;
        rtn = _this->ProgressCallback( dltotal, dlnow, ultotal, ulnow );
    }

    return rtn;
}
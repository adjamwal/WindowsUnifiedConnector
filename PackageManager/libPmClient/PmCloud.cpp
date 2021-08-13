#include "PmCloud.h"

PmCloud::PmCloud( IPmHttp& http )
    : m_http( http )
    , m_certs( { 0 } )
    , m_shutdownFunc( [] { return false; } )
    , m_userAgent( "PackageManager" )
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
    m_shutdownFunc = shutdownFunc;
}

int32_t PmCloud::Checkin( const std::string& payload, std::string& response )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_http.Init( _ProgressCallback, this, m_userAgent );
    m_http.SetCerts( m_certs );
    m_http.SetToken( m_token );

    int32_t httpStatusResponse = 0;
    m_http.HttpPost( m_uri, (void*)payload.c_str(), payload.length() + 1, response, httpStatusResponse );
    m_http.Deinit();
    return httpStatusResponse;
}

int32_t PmCloud::Get( const std::string& url, std::string& response, int32_t& httpStatusResponse )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_http.Init( _ProgressCallback, this, m_userAgent );
    m_http.SetCerts( m_certs );
    m_http.SetToken( m_token );

    m_http.HttpGet( url, response, httpStatusResponse );
    m_http.Deinit();

    return httpStatusResponse;
}

int32_t PmCloud::Post( const std::string& url, void* payload, size_t payloadSize, std::string& response, int32_t& httpStatusResponse )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_http.Init( _ProgressCallback, this, m_userAgent );
    m_http.SetCerts( m_certs );
    m_http.SetToken( m_token );
    
    m_http.HttpPost( url, payload, payloadSize, response, httpStatusResponse );
    m_http.Deinit();

    return httpStatusResponse;
}

int32_t PmCloud::DownloadFile( const std::string& uri, const std::filesystem::path& filename )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    
    m_http.Init( _ProgressCallback, this, m_userAgent );

    m_http.SetCerts( m_certs );
    m_http.SetToken( m_token );

    int32_t httpStatusResponse = 0;
    m_http.HttpDownload( uri, filename, httpStatusResponse );
    m_http.Deinit();

    return httpStatusResponse;
}

int PmCloud::ProgressCallback( PM_TYPEOF_OFF_T dltotal, PM_TYPEOF_OFF_T dlnow, PM_TYPEOF_OFF_T ultotal, PM_TYPEOF_OFF_T ulnow )
{
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

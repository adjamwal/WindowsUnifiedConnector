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

void PmCloud::SetCheckinUri( const std::string& uri )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_checkinUri = uri;
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

bool PmCloud::Checkin( const std::string& payload, std::string& responseContent, PmHttpExtendedResult& eResult )
{
    return Post( m_checkinUri, payload.c_str(), payload.length() + 1, responseContent, eResult );
}

bool PmCloud::Get( const std::string& url, std::string& responseContent, PmHttpExtendedResult& eResult )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( m_http.Init( _ProgressCallback, this, m_userAgent, eResult ) &&
        m_http.SetCerts( m_certs, eResult ) &&
        m_http.SetToken( m_token, eResult ) )
    {
        m_http.HttpGet( url, responseContent, eResult );
    }

    m_http.Deinit();

    return eResult.httpResponseCode >= 200 && eResult.httpResponseCode <= 299;
}

bool PmCloud::Post( const std::string& url, const void* payload, size_t payloadSize, std::string& responseContent, PmHttpExtendedResult& eResult )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( m_http.Init( _ProgressCallback, this, m_userAgent, eResult ) &&
        m_http.SetCerts( m_certs, eResult ) &&
        m_http.SetToken( m_token, eResult ) )
    {
        m_http.HttpPost( url, payload, payloadSize, responseContent, eResult );
    }

    m_http.Deinit();

    return eResult.httpResponseCode >= 200 && eResult.httpResponseCode <= 299;
}

bool PmCloud::DownloadFile( const std::string& uri, const std::filesystem::path& filename, PmHttpExtendedResult& eResult )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( m_http.Init( _ProgressCallback, this, m_userAgent, eResult ) &&
        m_http.SetCerts( m_certs, eResult ) &&
        m_http.SetToken( m_token, eResult ) )
    {
        m_http.HttpDownload( uri, filename, eResult );
    }

    m_http.Deinit();

    return eResult.httpResponseCode >= 200 && eResult.httpResponseCode <= 299;
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

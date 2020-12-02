#include "CheckinManifestRetriever.h"
#include "PmLogger.h"
#include "PmCloud.h"

CheckinManifestRetriever::CheckinManifestRetriever( IPmCloud& cloud, ITokenAdapter& tokenAdapter, ICertsAdapter& certsAdapter )
    : m_cloud( cloud )
    , m_tokenAdapter( tokenAdapter )
    , m_certsAdapter( certsAdapter )
{
}

CheckinManifestRetriever::~CheckinManifestRetriever()
{
}

std::string CheckinManifestRetriever::GetCheckinManifestFrom( std::string uri, std::string payload )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    std::string response;
    bool checkinFailed = false;

    try {
        response = InternalGetCheckinManifestFrom( uri, payload );
    }
    catch ( std::exception& ex ) {
        checkinFailed = true;
    }

    if ( checkinFailed ) {
        m_tokenAdapter.Refresh();
        response = InternalGetCheckinManifestFrom( uri, payload );
    }

    return response;
}

std::string CheckinManifestRetriever::InternalGetCheckinManifestFrom( std::string uri, std::string payload )
{
    m_cloud.SetUri( uri );
    m_cloud.SetToken( m_tokenAdapter.GetAccessToken() );
    m_cloud.SetCerts( m_certsAdapter.GetCertsList() );

    LOG_DEBUG( "Checkin uri:%s, payload:%s", uri.c_str(), payload.c_str() );

    std::string response;
    int32_t respStatus = m_cloud.Checkin( payload, response );

    if ( respStatus != 200 ) {
        std::string s = __FUNCTION__ ": Http Post status ";
        s += std::to_string( respStatus );
        throw std::exception( s.c_str() );
    }

    return response;
}
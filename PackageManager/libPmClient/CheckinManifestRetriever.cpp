#include "CheckinManifestRetriever.h"
#include "PmLogger.h"
#include "PmCloud.h"

CheckinManifestRetriever::CheckinManifestRetriever( IPmCloud& cloud, IUcidAdapter& ucidAdapter, ICertsAdapter& certsAdapter )
    : m_cloud( cloud )
    , m_ucidAdapter( ucidAdapter )
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
    int32_t respStatus;

    respStatus = InternalGetCheckinManifestFrom( uri, payload, response );
    
    if ( respStatus != 200 ) {
        if ( respStatus != 401 ) {
            HandleHttpError( respStatus );
        }
        else {
            m_ucidAdapter.Refresh();
            respStatus = InternalGetCheckinManifestFrom( uri, payload, response );

            if ( respStatus != 200 ) {
                HandleHttpError( respStatus );
            }
        }
    }

    return response;
}

int32_t CheckinManifestRetriever::InternalGetCheckinManifestFrom( std::string& uri, std::string& payload, std::string& response )
{
    std::string token = m_ucidAdapter.GetAccessToken();
    if ( token.empty() ) {
        return 0;
    }

    m_cloud.SetUri( uri );
    m_cloud.SetToken( token );
    m_cloud.SetCerts( m_certsAdapter.GetCertsList() );

    LOG_DEBUG( "Checkin uri:%s, payload:%s", uri.c_str(), payload.c_str() );

    return m_cloud.Checkin( payload, response );
}

void CheckinManifestRetriever::HandleHttpError( int32_t respStatus )
{
    std::string s = __FUNCTION__ ": Http Post status ";
    s += std::to_string( respStatus );
    throw std::exception( s.c_str() );
}
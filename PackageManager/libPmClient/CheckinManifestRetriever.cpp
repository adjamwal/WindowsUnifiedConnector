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
    std::string responseContent;
    PmHttpExtendedResult eResult = {};

    InternalGetCheckinManifestFrom( uri, payload, responseContent, eResult );

    if( eResult.httpResponseCode != 200 ) {
        if( eResult.httpResponseCode != 401 ) {
            ThrowHttpError( eResult );
        }
        else {
            m_ucidAdapter.Refresh();
            InternalGetCheckinManifestFrom( uri, payload, responseContent, eResult );

            if( eResult.httpResponseCode != 200 ) {
                ThrowHttpError( eResult );
            }
        }
    }

    return responseContent;
}

bool CheckinManifestRetriever::InternalGetCheckinManifestFrom( std::string& uri, std::string& payload, std::string& responseContent, PmHttpExtendedResult& eResult )
{
    eResult = {};
    std::string token = m_ucidAdapter.GetAccessToken();

    if( token.empty() ) {
        eResult.subErrorCode = ( int32_t )USER_DEF_SUB_ERROR::SC_MISSING_TOKEN;
        return false;
    }

    m_cloud.SetCheckinUri( uri );
    m_cloud.SetToken( token );
    m_cloud.SetCerts( m_certsAdapter.GetCertsList() );

    LOG_DEBUG( "Checkin uri:%s, payload:%s", uri.c_str(), payload.c_str() );

    return m_cloud.Checkin( payload, responseContent, eResult );
}

void CheckinManifestRetriever::ThrowHttpError( PmHttpExtendedResult& eResult )
{
    std::string s = "Checkin status code: " +
        std::to_string( eResult.httpResponseCode ) + 
        ", sub_code " + std::to_string( eResult.subErrorCode );

    throw std::exception( s.c_str() );
}

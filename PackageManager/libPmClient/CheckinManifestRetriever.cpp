#include "CheckinManifestRetriever.h"
#include "PmLogger.h"
#include "IUcidAdapter.h"
#include "ICertsAdapter.h"
#include "IPmCloud.h"
#include "IPmConfig.h"
#include "IPmHttp.h"

CheckinManifestRetriever::CheckinManifestRetriever( IPmCloud& cloud, 
    IUcidAdapter& ucidAdapter, 
    ICertsAdapter& certsAdapter, 
    IPmConfig& config )
    : m_cloud( cloud )
    , m_ucidAdapter( ucidAdapter )
    , m_certsAdapter( certsAdapter )
    , m_config( config )
{
}

CheckinManifestRetriever::~CheckinManifestRetriever()
{
}

std::string CheckinManifestRetriever::GetCheckinManifest( std::string payload )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    std::string responseContent;
    PmHttpExtendedResult eResult = {};
    std::string uri = m_config.GetCloudCheckinUri();

    InternalGetCheckinManifestFrom( uri, payload, responseContent, eResult );

    if( eResult.httpResponseCode != 200 ) {
        if( eResult.httpResponseCode != 401 ) {
            ThrowHttpError( eResult );
        }
        else {
            m_ucidAdapter.Refresh();
            // The checkin url could potentially change after UCID refreshes
            uri = m_config.GetCloudCheckinUri();
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

#include "CatalogListRetriever.h"
#include "IUcLogger.h"
#include "PmCloud.h"

CatalogListRetriever::CatalogListRetriever( IPmCloud& cloud, IUcidAdapter& ucidAdapter, ICertsAdapter& certsAdapter, IPmConfig& config )
    : m_cloud( cloud )
    , m_ucidAdapter( ucidAdapter )
    , m_certsAdapter( certsAdapter )
    , m_config( config )
{
}

CatalogListRetriever::~CatalogListRetriever()
{
}

std::string CatalogListRetriever::GetCloudCatalog()
{
    std::string uri = m_config.GetCloudCatalogUri();

    std::string catalogData;
    int32_t respStatus = InternalGetCloudCatalogFrom( uri, catalogData );

    if( respStatus != 200 ) {
        if( respStatus != 401 ) {
            HandleHttpError( respStatus );
        }
        else {
            m_ucidAdapter.Refresh();
            respStatus = InternalGetCloudCatalogFrom( uri, catalogData );

            if( respStatus != 200 ) {
                HandleHttpError( respStatus );
            }
        }
    }

    return catalogData;
}

int32_t CatalogListRetriever::InternalGetCloudCatalogFrom( std::string& uri, std::string& responseData )
{
    std::string token = m_ucidAdapter.GetAccessToken();
    if( token.empty() ) {
        return 0;
    }

    m_cloud.SetToken( token );
    m_cloud.SetCerts( m_certsAdapter.GetCertsList() );

    int httpStatusResponse = 0;
    int curlErrCode = m_cloud.Get( uri, responseData, httpStatusResponse );
    LOG_DEBUG( "m_cloud.Get:\n------uri=%s\n------responseData=%s\n------curlErrCode=%d\n------httpStatusResponse=%d\n", 
        uri.c_str(), responseData.c_str(), curlErrCode, httpStatusResponse );

    return httpStatusResponse;
}

void CatalogListRetriever::HandleHttpError( int32_t respStatus )
{
    std::string s = __FUNCTION__ ": Http Get status ";
    s += std::to_string( respStatus );
    throw std::exception( s.c_str() );
}

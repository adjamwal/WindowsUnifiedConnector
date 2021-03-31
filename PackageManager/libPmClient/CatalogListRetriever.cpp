#include "CatalogListRetriever.h"
#include "IUcLogger.h"
#include "PmCloud.h"
#include <algorithm>
#include <StringUtil.h>

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
    int32_t httpStatusResponse = InternalGetCloudCatalogFrom( uri, catalogData );

    if( httpStatusResponse != 200 ) {
        if( httpStatusResponse != 401 ) {
            HandleHttpError( httpStatusResponse );
        }
        else {
            m_ucidAdapter.Refresh();
            httpStatusResponse = InternalGetCloudCatalogFrom( uri, catalogData );

            if( httpStatusResponse != 200 ) {
                HandleHttpError( httpStatusResponse );
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
    m_cloud.Get( uri, responseData, httpStatusResponse );
    LOG_DEBUG( "m_cloud.Get:\n------uri=%s\n------httpStatusResponse=%d\n------responseData=%s\n", 
        uri.c_str(), httpStatusResponse, responseData.c_str() );

    return httpStatusResponse;
}

void CatalogListRetriever::HandleHttpError( int32_t httpStatusResponse )
{
    std::string s = __FUNCTION__ ": Http Get status ";
    s += std::to_string( httpStatusResponse );
    throw std::exception( s.c_str() );
}

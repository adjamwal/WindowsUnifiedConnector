#include "CatalogListRetriever.h"
#include "PmLogger.h"
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
    std::lock_guard<std::mutex> lock( m_mutex );
    std::string response;
    int32_t respStatus;

    LOG_DEBUG( "Enter: CatalogListRetriever::GetCloudCatalog" );

    std::string uri = m_config.GetCloudCatalogUri();

    respStatus = InternalGetCloudCatalogFrom( uri, response );

    if( respStatus != 200 ) {
        if( respStatus != 401 ) {
            HandleHttpError( respStatus );
        }
        else {
            m_ucidAdapter.Refresh();
            respStatus = InternalGetCloudCatalogFrom( uri, response );

            if( respStatus != 200 ) {
                HandleHttpError( respStatus );
            }
        }
    }

    LOG_DEBUG( "Exit: CatalogListRetriever::GetCloudCatalog" );

    return response;
}

int32_t CatalogListRetriever::InternalGetCloudCatalogFrom( std::string& uri, std::string& response )
{
    LOG_DEBUG( "Enter: CatalogListRetriever::InternalGetCloudCatalogFrom %s", uri.c_str() );

    std::string token = m_ucidAdapter.GetAccessToken();
    if( token.empty() ) {
        return 0;
    }

    int32_t httpRetCode;

    LOG_DEBUG( "Setting Token" );
    m_cloud.SetToken( token );

    LOG_DEBUG( "Setting Certs" );
    m_cloud.SetCerts( m_certsAdapter.GetCertsList() );

    LOG_DEBUG( "HTTP Get" );
    return m_cloud.Get( uri, response, httpRetCode );
    LOG_DEBUG( "m_cloud.Get: %s %s %d", uri.c_str(), response.c_str(), httpRetCode );

    LOG_DEBUG( "Exit: CatalogListRetriever::InternalGetCloudCatalogFrom" );
}

void CatalogListRetriever::HandleHttpError( int32_t respStatus )
{
    std::string s = __FUNCTION__ ": Http Get status ";
    s += std::to_string( respStatus );
    throw std::exception( s.c_str() );
}

#include "CatalogListRetriever.h"
#include "PmLogger.h"
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
    PmHttpExtendedResult eResult = {};
    InternalGetCloudCatalogFrom( uri, catalogData, eResult );

    if( eResult.httpResponseCode != 200 ) {
        if( eResult.httpResponseCode != 401 ) {
            ThrowHttpError( eResult );
        }
        else {
            m_ucidAdapter.Refresh();
            InternalGetCloudCatalogFrom( uri, catalogData, eResult );

            if( eResult.httpResponseCode != 200 ) {
                ThrowHttpError( eResult );
            }
        }
    }

    return catalogData;
}

int32_t CatalogListRetriever::InternalGetCloudCatalogFrom( std::string& uri, std::string& responseData, PmHttpExtendedResult& eResult )
{
    eResult = {};
    std::string token = m_ucidAdapter.GetAccessToken();
    if( token.empty() ) {
        eResult.subErrorCode = ( int32_t )USER_DEF_SUB_ERROR::SC_MISSING_TOKEN;
        return false;
    }

    m_cloud.SetToken( token );

    m_cloud.Get( uri, responseData, eResult );
    LOG_DEBUG( "m_cloud.Get:\n------uri=%s\n------httpStatusResponse=%d\n------subError=%d\n------responseData=%s\n", 
        uri.c_str(), eResult.httpResponseCode, eResult.subErrorCode, responseData.c_str() );

    return eResult.httpResponseCode;
}

void CatalogListRetriever::ThrowHttpError( PmHttpExtendedResult& eResult )
{
    std::string s = "CloudCatalog status code: " +
        std::to_string( eResult.httpResponseCode ) +
        ", sub_code " + std::to_string( eResult.subErrorCode );

    throw std::exception( s.c_str() );
}

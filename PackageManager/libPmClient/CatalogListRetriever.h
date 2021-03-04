#pragma once

#include "ICatalogListRetriever.h"
#include "IUcidAdapter.h"
#include "ICertsAdapter.h"
#include "IPmCloud.h"
#include "IPmConfig.h"
#include <mutex>

class CatalogListRetriever : public ICatalogListRetriever
{
public:
    CatalogListRetriever( IPmCloud& cloud, IUcidAdapter& ucidAdapter, ICertsAdapter& certsAdapter, IPmConfig& config );
    virtual ~CatalogListRetriever();

    std::string GetCloudCatalog() override;
private:
    IPmCloud& m_cloud;
    IUcidAdapter& m_ucidAdapter;
    ICertsAdapter& m_certsAdapter;
    IPmConfig& m_config;

    int32_t InternalGetCloudCatalogFrom( std::string& uri, std::string& response );
    void HandleHttpError( int32_t respStatus );
};

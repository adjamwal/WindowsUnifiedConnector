#pragma once

#include "ICheckinManifestRetriever.h"
#include "IUcidAdapter.h"
#include "ICertsAdapter.h"
#include "IPmCloud.h"
#include <mutex>

class CheckinManifestRetriever : public ICheckinManifestRetriever
{
public:
    CheckinManifestRetriever( IPmCloud& cloud, IUcidAdapter& ucidAdapter, ICertsAdapter& certsAdapter );
    virtual ~CheckinManifestRetriever();

    std::string GetCheckinManifestFrom( std::string uri, std::string payload ) override;
private:
    IPmCloud& m_cloud;
    IUcidAdapter& m_ucidAdapter;
    ICertsAdapter& m_certsAdapter;
    std::mutex m_mutex;

    int32_t InternalGetCheckinManifestFrom( std::string& uri, std::string& payload, std::string& response );
    void HandleHttpError( int32_t respStatus );
};

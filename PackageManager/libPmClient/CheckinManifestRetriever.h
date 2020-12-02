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

    virtual std::string GetCheckinManifestFrom( std::string uri, std::string payload ) override;
private:
    IPmCloud& m_cloud;
    IUcidAdapter& m_ucidAdapter;
    ICertsAdapter& m_certsAdapter;
    std::mutex m_mutex;

    std::string InternalGetCheckinManifestFrom( std::string uri, std::string payload );
};

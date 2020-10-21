#pragma once

#include "ICheckinManifestRetriever.h"
#include "ITokenAdapter.h"
#include "ICertsAdapter.h"
#include "IPmCloud.h"
#include <mutex>

class CheckinManifestRetriever : public ICheckinManifestRetriever
{
public:
    CheckinManifestRetriever( IPmCloud& cloud, ITokenAdapter& tokenAdapter, ICertsAdapter certsAdapter );
    virtual ~CheckinManifestRetriever();

    virtual std::string GetCheckinManifestFrom( std::string uri ) override;
private:
    IPmCloud& m_cloud;
    ITokenAdapter& m_tokenAdapter;
    ICertsAdapter& m_certsAdapter;
    std::mutex m_mutex;
};

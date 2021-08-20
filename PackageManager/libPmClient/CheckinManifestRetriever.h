#pragma once

#include "ICheckinManifestRetriever.h"
#include <string>
#include <mutex>

class IUcidAdapter;
class ICertsAdapter;
class IPmCloud;
class IPmConfig;
struct PmHttpExtendedResult;

class CheckinManifestRetriever : public ICheckinManifestRetriever
{
public:
    CheckinManifestRetriever( IPmCloud& cloud, IUcidAdapter& ucidAdapter, ICertsAdapter& certsAdapter, IPmConfig& config );
    virtual ~CheckinManifestRetriever();

    std::string GetCheckinManifest( std::string payload ) override;
private:
    IPmCloud& m_cloud;
    IUcidAdapter& m_ucidAdapter;
    ICertsAdapter& m_certsAdapter;
    IPmConfig& m_config;
    std::mutex m_mutex;

    bool InternalGetCheckinManifestFrom( std::string& uri, std::string& payload, std::string& response, PmHttpExtendedResult& eResult );
    void ThrowHttpError( PmHttpExtendedResult& eResult );
};

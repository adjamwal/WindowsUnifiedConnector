#pragma once

#include "ICloudEventPublisher.h"
#include "ICloudEventStorage.h"
#include "IPmCloud.h"
#include "IPmConfig.h"
#include <mutex>
#include <string>

class CloudEventPublisher final : public ICloudEventPublisher
{
public:
    CloudEventPublisher( IPmCloud& pmCloud, ICloudEventStorage& eventStorage, IPmConfig& pmConfig );
    ~CloudEventPublisher();

    void SetToken( const std::string& token ) override;
    int32_t Publish( ICloudEventBuilder& event ) override;
    int32_t PublishFailedEvents() override;
private:
    IPmCloud& m_pmCloud;
    ICloudEventStorage& m_eventStorage;
    IPmConfig& m_pmConfig;
    std::string m_eventEndpointUrl;
    std::mutex m_mutex;

    int32_t InternalPublish( const std::string& eventJson );
};

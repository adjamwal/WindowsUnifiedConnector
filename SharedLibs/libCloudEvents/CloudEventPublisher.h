#pragma once

#include "ICloudEventPublisher.h"
#include "ICloudEventStorage.h"
#include "IPmHttp.h"
#include "IPmConfig.h"
#include <mutex>
#include <string>

class CloudEventPublisher final : public ICloudEventPublisher
{
public:
    CloudEventPublisher( IPmHttp& httpAdapter, ICloudEventStorage& eventStorage, IPmConfig& pmConfig );
    ~CloudEventPublisher();

    void SetToken( const std::string& token ) override;
    int32_t Publish( ICloudEventBuilder& event ) override;
    int32_t PublishFailedEvents() override;
private:
    IPmHttp& m_httpAdapter;
    ICloudEventStorage& m_eventStorage;
    IPmConfig& m_pmConfig;
    std::string m_eventEndpointUrl;
    std::mutex m_mutex;

    int32_t InternalPublish( const std::string& eventJson );
};

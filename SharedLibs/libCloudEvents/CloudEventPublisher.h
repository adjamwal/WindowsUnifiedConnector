#pragma once

#include "ICloudEventPublisher.h"
#include "ICloudEventStorage.h"
#include "IPmHttp.h"
#include <mutex>
#include <string>

#define CLOUD_EVENT_PUBLISHING_URL "https://4m2294wzqf.execute-api.us-west-1.amazonaws.com/dev/identify/event"

class CloudEventPublisher final : public ICloudEventPublisher
{
public:
    CloudEventPublisher( IPmHttp& httpAdapter, ICloudEventStorage& eventStorage, const std::string& eventEndpointUrl );
    ~CloudEventPublisher();

    void SetToken( const std::string& token ) override;
    int32_t Publish( ICloudEventBuilder& event ) override;
    int32_t PublishFailedEvents() override;
private:
    IPmHttp& m_httpAdapter;
    ICloudEventStorage& m_eventStorage;
    std::string m_eventEndpointUrl;
    std::mutex m_mutex;

    int32_t InternalPublish( const std::string& eventJson );
};

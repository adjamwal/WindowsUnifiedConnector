#pragma once

#include "ICloudEventPublisher.h"
#include "ICloudEventStorage.h"
#include "IPmHttp.h"
#include <mutex>
#include <string>

class CloudEventPublisher final : public ICloudEventPublisher
{
public:
    CloudEventPublisher( IPmHttp& httpAdapter, ICloudEventStorage& eventStorage, const std::string& eventEndpointUrl );
    ~CloudEventPublisher();

    int32_t Publish( ICloudEventBuilder& event ) override;
    int32_t Publish( const std::string& eventJson ) override;
    int32_t PublishFailedEvents() override;
private:
    IPmHttp& m_httpAdapter;
    ICloudEventStorage& m_eventStorage;
    std::string m_eventEndpointUrl;

    std::mutex m_mutex;
};

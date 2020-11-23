#pragma once

#include "ICloudEventPublisher.h"
#include "IHttpAdapter.h"
#include <mutex>
#include <string>

class CloudEventPublisher final : public ICloudEventPublisher
{
public:
    CloudEventPublisher( IHttpAdapter& httpAdapter, const std::string& eventEndpointUrl );
    ~CloudEventPublisher();

    int32_t Publish( ICloudEventBuilder& event ) override;

private:
    IHttpAdapter& m_httpAdapter;
    std::string m_eventEndpointUrl;

    std::mutex m_mutex;
};

#pragma once

#include "ICloudEventPublisher.h"
#include "IPmHttp.h"
#include <mutex>
#include <string>

class CloudEventPublisher final : public ICloudEventPublisher
{
public:
    CloudEventPublisher( IPmHttp& httpAdapter, const std::string& eventEndpointUrl );
    ~CloudEventPublisher();

    int32_t Publish( ICloudEventBuilder& event ) override;
    int32_t Publish( const std::string& eventJson ) override;

private:
    IPmHttp& m_httpAdapter;
    std::string m_eventEndpointUrl;

    std::mutex m_mutex;
};

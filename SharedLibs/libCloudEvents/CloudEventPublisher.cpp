/**************************************************************************
 * Copyright (c) 2020, Cisco Systems, All Rights Reserved
 ***************************************************************************
 *
 * @file : CloudEventPublisher.cpp
 *
 ***************************************************************************
 *
 * Implements UC cloud event publisher
 *
 ***************************************************************************/
#include "CloudEventPublisher.h"
#include <Windows.h>
#include "IUcLogger.h"
#include "json\json.h"

CloudEventPublisher::CloudEventPublisher( IPmHttp& httpAdapter, ICloudEventStorage& eventStorage, const std::string& eventEndpointUrl )
    : m_httpAdapter( httpAdapter )
    , m_eventStorage( eventStorage )
    , m_eventEndpointUrl( eventEndpointUrl )
{
}

CloudEventPublisher::~CloudEventPublisher()
{
}

void CloudEventPublisher::SetToken( const std::string& token )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_httpAdapter.SetToken( token );
}

int32_t CloudEventPublisher::Publish( ICloudEventBuilder& event )
{
    std::string eventPayload = event.Build();

    return InternalPublish( eventPayload );
}

int32_t CloudEventPublisher::PublishFailedEvents()
{
    int32_t publishReturn = 0;
    std::vector<std::string> events;

    {
        std::lock_guard<std::mutex> lock( m_mutex );
        events = m_eventStorage.ReadEvents();
    }

    for ( auto&& e : events )
    {
        publishReturn = InternalPublish( e );
    }

    return publishReturn;
}

int32_t CloudEventPublisher::InternalPublish( const std::string& eventJson )
{
    int32_t postReturn = 0;
    std::string eventResponse;
    int32_t httpReturn;

    std::lock_guard<std::mutex> lock( m_mutex );

    postReturn = m_httpAdapter.HttpPost(
        m_eventEndpointUrl,
        ( void* )eventJson.c_str(),
        eventJson.length(),
        eventResponse,
        httpReturn );

    if( postReturn || httpReturn < 200 || httpReturn >= 300 )
    {
        m_eventStorage.SaveEvent( eventJson );
    }

    return postReturn;
}

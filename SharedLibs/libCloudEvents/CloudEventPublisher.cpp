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
#include "pch.h"

#include "CloudEventPublisher.h"
#include <Windows.h>
#include "IUcLogger.h"
#include "json\json.h"

CloudEventPublisher::CloudEventPublisher( IHttpAdapter& httpAdapter, const std::string& eventEndpointUrl )
    : m_httpAdapter( httpAdapter )
    , m_eventEndpointUrl( eventEndpointUrl )
{
}

CloudEventPublisher::~CloudEventPublisher()
{
}

int32_t CloudEventPublisher::Publish( ICloudEventBuilder& event )
{
    std::string eventPayload = event.Build();

    return Publish( eventPayload );
}

int32_t CloudEventPublisher::Publish( const std::string& eventJson )
{
    std::string eventResponse;
    int32_t httpReturn;

    std::lock_guard<std::mutex> lock( m_mutex );

    return m_httpAdapter.HttpPost(
        m_eventEndpointUrl,
        ( void* )eventJson.c_str(),
        eventJson.length(),
        eventResponse,
        httpReturn );
}

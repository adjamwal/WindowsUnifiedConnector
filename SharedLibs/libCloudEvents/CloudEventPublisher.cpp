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
    std::lock_guard<std::mutex> lock( m_mutex );

    std::string eventPayload = event.Build();
    std::string eventResponse;
    int32_t httpReturn;

    int32_t result = m_httpAdapter.HttpPost( 
        m_eventEndpointUrl, 
        ( void* )eventPayload.c_str(), 
        eventPayload.length(), 
        eventResponse, 
        httpReturn );

    return result;
}
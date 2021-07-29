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
#include "CloudEventBuilder.h"
#include <Windows.h>
#include "PmLogger.h"
#include "PmHttp.h"
#include "json\json.h"

CloudEventPublisher::CloudEventPublisher( IPmCloud& pmCloud, ICloudEventStorage& eventStorage, IPmConfig& pmConfig )
    : m_pmCloud( pmCloud )
    , m_eventStorage( eventStorage )
    , m_pmConfig( pmConfig )
{
}

CloudEventPublisher::~CloudEventPublisher()
{
}

void CloudEventPublisher::SetToken( const std::string& token )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_pmCloud.SetToken( token );
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
        events = m_eventStorage.ReadAndRemoveEvents();
    }

    for ( auto&& e : events )
    {
        CloudEventBuilder eventBuilder;
        if ( CloudEventBuilder::Deserialize( eventBuilder, e ) )
        {
            publishReturn |= InternalPublish( eventBuilder.Build() );
        }
    }

    return publishReturn;
}

int32_t CloudEventPublisher::InternalPublish( const std::string& eventJson )
{
    std::string eventResponse;
    PmHttpExtendedResult eResult = {};

    std::lock_guard<std::mutex> lock( m_mutex );

    m_pmCloud.Post(
        m_pmConfig.GetCloudEventUri(),
        ( void* )eventJson.c_str(),
        eventJson.length(),
        eventResponse,
        eResult );

    if( ( eResult.httpResponseCode < 200 || eResult.httpResponseCode >= 300 ) && 
        ( eResult.httpResponseCode != 400 ) ) {
        m_eventStorage.SaveEvent( eventJson );
    }

    return eResult.httpResponseCode;
}

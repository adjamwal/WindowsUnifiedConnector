/**************************************************************************
 * Copyright (c) 2020, Cisco Systems, All Rights Reserved
 ***************************************************************************
 *
 * @file : CloudEventBuilder.cpp
 *
 ***************************************************************************
 *
 * Implements UC cloud event builder
 *
 ***************************************************************************/
#define _CRT_SECURE_NO_WARNINGS

#include "CloudEventBuilder.h"
#include <Windows.h>
#include "IUcLogger.h"
#include "json\json.h"
#include <chrono>
#include <ctime>
#include <iomanip>

CloudEventBuilder::CloudEventBuilder()
{
    Reset();
}

CloudEventBuilder::~CloudEventBuilder()
{
}

ICloudEventBuilder& CloudEventBuilder::FromJson( const std::string& eventJson )
{
    Reset();
    Deserialize( eventJson );
    UpdateEventTime();
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithUCID( const std::string& ucid )
{
    m_ucid = ucid;
    UpdateEventTime();
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithType( CloudEventType evtype )
{
    m_evtype = evtype;
    UpdateEventTime();
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithPackage( const std::string& name, const std::string& version )
{
    m_packageName = name;
    m_packageVersion = version;
    UpdateEventTime();
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithPackageID( const std::string& idAsNameAndVersion )
{
    std::istringstream original( idAsNameAndVersion );
    std::vector<std::string> parts;
    std::string s;

    while( std::getline( original, s, '/' ) ) {
        parts.push_back( s );
    }

    if( parts.size() > 0 ) m_packageName = parts[ 0 ];
    if( parts.size() > 1 ) m_packageVersion = parts[ 1 ];

    UpdateEventTime();
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithError( int code, const std::string& message )
{
    m_errCode = code;
    m_errMessage = message;
    UpdateEventTime();
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithOldFile( const std::string& path, const std::string& hash, int size )
{
    m_oldPath = path;
    m_oldHash = hash;
    m_oldSize = size;
    UpdateEventTime();
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithNewFile( const std::string& path, const std::string& hash, int size )
{
    m_newPath = path;
    m_newHash = hash;
    m_newSize = size;
    UpdateEventTime();
    return *this;
}

std::string CloudEventBuilder::GetPackageName()
{
    return m_packageName;
}

std::string CloudEventBuilder::GetPackageVersion()
{
    return m_packageVersion;
}

std::string CloudEventBuilder::Build()
{
    std::string result = Serialize();
    LOG_DEBUG( __FUNCTION__ ": serialized event: %s", result.c_str() );
    Reset();
    return result;
}

void CloudEventBuilder::Reset()
{
    LOG_DEBUG( __FUNCTION__ ": Enter" );

    m_ucid = "";
    m_evtype = CloudEventType( 0 );
    m_packageName = "";
    m_packageVersion = "";
    m_errCode = 0;
    m_errMessage = "";
    m_oldPath = "";
    m_oldHash = "";
    m_oldSize = 0;
    m_newPath = "";
    m_newHash = "";
    m_newSize = 0;
    UpdateEventTime();

    LOG_DEBUG( __FUNCTION__ ": Exit" );
}

//https://stackoverflow.com/questions/54325137/c-rfc3339-timestamp-with-milliseconds-using-stdchrono
std::string CloudEventBuilder::Now_RFC3339()
{
    const auto now_ms = std::chrono::time_point_cast< std::chrono::milliseconds >( std::chrono::system_clock::now() );
    const auto now_s = std::chrono::time_point_cast< std::chrono::seconds >( now_ms );
    const auto millis = now_ms - now_s;
    const auto c_now = std::chrono::system_clock::to_time_t( now_s );

    std::stringstream ss;
    ss  << std::put_time( gmtime( &c_now ), "%FT%T" ) 
        << '.' 
        << std::setfill( '0' ) << std::setw( 2 ) << millis.count() / 100
        << std::put_time( gmtime( &c_now ), "%z" );

    std::string result = ss.str();
    result.insert( result.end() - 2, ':' );

    return result;
}

void CloudEventBuilder::UpdateEventTime()
{
    m_tse = Now_RFC3339();
}

std::string CloudEventBuilder::Serialize()
{
    Json::Value event;
    event[ "type" ] = CloudEventString( m_evtype );
    event[ "package" ] = m_packageName + "/" + m_packageVersion;

    if( m_evtype == pkgreconfig )
    {
        if( m_oldPath.length() > 0 )
        {
            Json::Value oldfilearr;
            Json::Value oldfile;
            oldfile[ "path" ] = m_oldPath;
            oldfile[ "sha256" ] = m_oldHash;
            oldfile[ "size" ] = m_oldSize;
            oldfilearr[ 0 ] = oldfile;
            event[ "old" ] = oldfilearr;
        }

        if( m_newPath.length() > 0 )
        {
            Json::Value newfilearr;
            Json::Value newfile;
            newfile[ "path" ] = m_newPath;
            newfile[ "sha256" ] = m_newHash;
            newfile[ "size" ] = m_newSize;
            newfilearr[ 0 ] = newfile;
            event[ "new" ] = newfilearr;
        }
    }

    if( m_errCode != 0 )
    {
        Json::Value error;
        error[ "code" ] = (unsigned)m_errCode;
        error[ "msg" ] = m_errMessage;
        event[ "err" ] = error;
    }

    event[ "tse" ] = m_tse;
    event[ "tstx" ] = Now_RFC3339();
    event[ "ucid" ] = m_ucid;

    Json::Value root;
    root[ "event" ] = event;

    return Json::writeString( Json::StreamWriterBuilder(), root );
}

void CloudEventBuilder::Deserialize( const std::string& eventJson )
{
    std::string orig_ucid = "";
    CloudEventType orig_evtype( CloudEventType( 0 ) );
    std::string orig_packageName = "";
    std::string orig_packageVersion = "";
    int orig_errCode(0);
    std::string orig_errMessage = "";
    std::string orig_oldPath = "";
    std::string orig_oldHash = "";
    int orig_oldSize(0);
    std::string orig_newPath = "";
    std::string orig_newHash = "";
    int orig_newSize(0);
    std::string orig_tse = "";

    Reset();

    try
    {
        

    }
    catch(...)
    {
        LOG_ERROR( __FUNCTION__ ": Error deserializing event: %s", eventJson.c_str() );
        return;
    }

    m_ucid = orig_ucid;
    m_evtype = orig_evtype;
    m_packageName = orig_packageName;
    m_packageVersion = orig_packageVersion;
    m_errCode = orig_errCode;
    m_errMessage = orig_errMessage;
    m_oldPath = orig_oldPath;
    m_oldHash = orig_oldHash;
    m_oldSize = orig_oldSize;
    m_newPath = orig_newPath;
    m_newHash = orig_newHash;
    m_newSize = orig_newSize;
    m_tse = orig_tse;
}

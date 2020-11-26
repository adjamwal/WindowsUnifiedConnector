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

ICloudEventBuilder& CloudEventBuilder::WithUCID( const std::string ucid )
{
    m_ucid = ucid;
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithType( CloudEventType evtype )
{
    m_evtype = evtype;
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithPackage( const std::string name, const std::string version )
{
    m_packageName = name;
    m_packageVersion = version;
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithError( int code, const std::string message )
{
    m_errCode = code;
    m_errMessage = message;
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithOldFile( const std::string path, const std::string hash, int size )
{
    m_oldPath = path;
    m_oldHash = hash;
    m_oldSize = size;
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithNewFile( const std::string path, const std::string hash, int size )
{
    m_newPath = path;
    m_newHash = hash;
    m_newSize = size;
    return *this;
}

std::string CloudEventBuilder::Build()
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
        error[ "code" ] = m_errCode;
        error[ "msg" ] = m_errMessage;
        event[ "err" ] = error;
    }

    event[ "tse" ] = Now_RFC3339();
    event[ "tsex" ] = event[ "tse" ];
    event[ "ucid" ] = m_ucid;

    Json::Value root;
    root[ "event" ] = event;

    std::string result = Json::writeString( Json::StreamWriterBuilder(), root );

    LOG_DEBUG( __FUNCTION__ ": %s", result.c_str() );

    return result;
}

void CloudEventBuilder::Reset()
{
    WLOG_DEBUG( __FUNCTION__ L": Enter" );

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

    WLOG_DEBUG( __FUNCTION__ L": Exit" );
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

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
    Deserialize( *this, eventJson );
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

bool CloudEventBuilder::Deserialize( ICloudEventBuilder& eventBuilder, const std::string& eventJson )
{
    std::string orig_ucid = "";
    std::string orig_tse = "";
    int orig_errCode( 0 );
    std::string orig_errMessage = "";
    CloudEventType orig_evtype( CloudEventType( 0 ) );
    std::string orig_packageName = "";
    std::string orig_oldPath = "";
    std::string orig_oldHash = "";
    int orig_oldSize(0);
    std::string orig_newPath = "";
    std::string orig_newHash = "";
    int orig_newSize(0);
    
    bool isValid = true;

    try
    {
        std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
        Json::Value root, event, error, oldfile, oldfilearr, newfile, newfilearr;
        std::string jsonError;

        if ( !eventJson.empty() ) {
            if ( jsonReader->parse( eventJson.c_str(), eventJson.c_str() + eventJson.length(), &root, &jsonError ) ) {
                event = root["event"];

                isValid = ExtractJsonString( event, "ucid", orig_ucid );

                isValid = ExtractJsonString( event, "tse", orig_tse );

                std::string typeString;
                isValid = ExtractJsonString( event, "type", typeString );

                if ( isValid )
                {
                    orig_evtype = ConvertCloudEventType( typeString );
                }

                isValid = ExtractJsonString( event, "package", orig_packageName );

                if ( !event["err"].isNull() ) {
                    error = event["err"];

                    isValid = ExtractJsonInt( error, "code", orig_errCode );
                    isValid = ExtractJsonString( error, "msg", orig_errMessage );
                }

                if ( !event["old"].isNull() && event["old"].isArray() && event["old"].size() == 1 ) {
                    oldfilearr = event["old"];
                    oldfile = oldfilearr[0];

                    isValid = ExtractJsonString( oldfile, "path", orig_oldPath );
                    isValid = ExtractJsonString( oldfile, "sha256", orig_oldHash );
                    isValid = ExtractJsonInt( oldfile, "size", orig_oldSize );
                }

                if ( !event["new"].isNull() && event["new"].isArray() && event["new"].size() == 1 ) {
                    newfilearr = event["new"];
                    newfile = newfilearr[0];

                    isValid = ExtractJsonString( newfile, "path", orig_newPath );
                    isValid = ExtractJsonString( newfile, "sha256", orig_newHash );
                    isValid = ExtractJsonInt( newfile, "size", orig_newSize );
                }
            }
            else {
                isValid = false;
                LOG_ERROR( "Json Parse error %s", jsonError.c_str() );
            }
        }
        else {
            isValid = false;
            LOG_ERROR( "json contents is empty" );
        }
    }
    catch( std::exception& ex )
    {
        isValid = false;
        LOG_ERROR( __FUNCTION__ ": Error deserializing event: %s", ex.what() );
    }

    if ( isValid )
    {
        eventBuilder
            .WithUCID( orig_ucid )
            .WithType( orig_evtype )
            .WithPackageID( orig_packageName )
            .WithOldFile( orig_oldPath, orig_oldHash, orig_oldSize )
            .WithNewFile( orig_newPath, orig_newHash, orig_newSize )
            .WithError( orig_errCode, orig_errMessage );
    }

    return isValid;
}

bool CloudEventBuilder::ExtractJsonInt( Json::Value& root, const std::string& attribute, int& dest )
{
    bool rtn = true;

    if ( root[attribute].isInt() ) {
        dest = root[attribute].asInt();
    }
    else {
        rtn = false;
        LOG_ERROR( "Invalid %s", attribute.c_str() );
    }

    return rtn;
}

bool CloudEventBuilder::ExtractJsonString( Json::Value& root, const std::string& attribute, std::string& dest )
{
    bool rtn = true;

    if ( root[attribute].isString() ) {
        dest = root[attribute].asString();
    }
    else {
        rtn = false;
        LOG_ERROR( "Invalid %s", attribute.c_str() );
    }

    return rtn;
}
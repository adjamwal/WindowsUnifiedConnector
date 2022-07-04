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
#include "CloudEventBuilder.h"
#include <Windows.h>
#include "PmLogger.h"
#include "JsonUtil.h"
#include "TimeUtil.h"
#include "StringUtil.h"
#include <iomanip>
#include <sstream>

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
    auto parts = StringUtil::Split( idAsNameAndVersion, '/' );

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

ICloudEventBuilder& CloudEventBuilder::WithSubError( int subErrCode, const std::string& subErrType )
{
    m_subErrCode = subErrCode;
    m_subErrType = subErrType;
    UpdateEventTime();
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithOldFile( const std::filesystem::path& path, const std::string& hash, uint64_t size )
{
    m_oldPath = path;
    m_oldHash = hash;
    m_oldSize = size;
    UpdateEventTime();
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithNewFile( const std::filesystem::path& path, const std::string& hash, uint64_t size )
{
    m_newPath = path;
    m_newHash = hash;
    m_newSize = size;
    UpdateEventTime();
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithFrom( const std::string& fromVersion )
{
    m_fromVersion = fromVersion;
    UpdateEventTime();
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithTse( const std::string& tse )
{
    m_tse = tse;
    return *this;
}

ICloudEventBuilder& CloudEventBuilder::WithTseNow()
{
    UpdateEventTime();
    return *this;
}

std::string CloudEventBuilder::GetPackageName() const
{
    return m_packageName;
}

std::string CloudEventBuilder::GetPackageVersion() const
{
    return m_packageVersion;
}

std::string CloudEventBuilder::GetRFC3339Tse() const
{
    return m_tse;
}

std::string CloudEventBuilder::Build()
{
    std::string result = Serialize();
    LOG_DEBUG( "Serialized event: %s", result.c_str() );
    return result;
}

void CloudEventBuilder::Reset()
{
    m_ucid = "";
    m_evtype = CloudEventType( 0 );
    m_packageName = "";
    m_packageVersion = "";
    m_errCode = 0;
    m_errMessage = "";
    m_subErrCode = 0;
    m_subErrType = "";
    m_oldPath = "";
    m_oldHash = "";
    m_oldSize = 0;
    m_newPath = "";
    m_newHash = "";
    m_newSize = 0;
    m_fromVersion = "";
    UpdateEventTime();
}

bool CloudEventBuilder::Deserialize( ICloudEventBuilder& eventBuilder, const std::string& eventJson )
{
    std::string orig_ucid = "";
    std::string orig_tse = "";
    int orig_errCode( 0 );
    std::string orig_errMessage = "";
    int orig_subErrCode( 0 );
    std::string orig_subErrType = "";
    CloudEventType orig_evtype( CloudEventType( 0 ) );
    std::string orig_packageName = "";
    std::string orig_oldPath = "";
    std::string orig_oldHash = "";
    int orig_oldSize( 0 );
    std::string orig_newPath = "";
    std::string orig_newHash = "";
    int orig_newSize( 0 );
    std::string orig_fromVersion = "";
    bool isValid = true;

    try
    {
        std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
        Json::Value root, event, error, oldfile, oldfilearr, newfile, newfilearr;
        std::string jsonError;

        if( !eventJson.empty() ) {
            if( jsonReader->parse( eventJson.c_str(), eventJson.c_str() + eventJson.length(), &root, &jsonError ) ) {
                event = root[ "event" ];

                isValid &= JsonUtil::ExtractJsonString( event, "ucid", orig_ucid );

                isValid &= JsonUtil::ExtractJsonString( event, "tse", orig_tse );

                std::string typeString;
                isValid &= JsonUtil::ExtractJsonString( event, "type", typeString );

                if( isValid )
                {
                    orig_evtype = ConvertCloudEventType( typeString );
                }

                isValid &= JsonUtil::ExtractJsonString( event, "package", orig_packageName );

                if( !event[ "from" ].isNull() ) {
                    isValid &= JsonUtil::ExtractJsonString( event, "from", orig_fromVersion );
                }

                if( !event[ "err" ].isNull() ) {
                    error = event[ "err" ];

                    isValid &= JsonUtil::ExtractJsonInt( error, "code", orig_errCode );
                    isValid &= JsonUtil::ExtractJsonString( error, "msg", orig_errMessage );
                    if( !error[ "sub_code" ].isNull() &&
                        JsonUtil::ExtractJsonInt( error, "sub_code", orig_subErrCode ) &&
                        orig_subErrCode != 0 )
                    {
                        isValid &= JsonUtil::ExtractJsonString( error, "sub_type", orig_subErrType );
                    }
                }

                if( !event[ "old" ].isNull() && event[ "old" ].isArray() && event[ "old" ].size() == 1 ) {
                    oldfilearr = event[ "old" ];
                    oldfile = oldfilearr[ 0 ];

                    isValid &= JsonUtil::ExtractJsonString( oldfile, "path", orig_oldPath );
                    isValid &= JsonUtil::ExtractJsonStringOptional( oldfile, "sha256", orig_oldHash );
                    isValid &= JsonUtil::ExtractJsonInt( oldfile, "size", orig_oldSize );
                }

                if( !event[ "new" ].isNull() && event[ "new" ].isArray() && event[ "new" ].size() == 1 ) {
                    newfilearr = event[ "new" ];
                    newfile = newfilearr[ 0 ];

                    isValid &= JsonUtil::ExtractJsonString( newfile, "path", orig_newPath );
                    isValid &= JsonUtil::ExtractJsonStringOptional( newfile, "sha256", orig_newHash );
                    isValid &= JsonUtil::ExtractJsonInt( newfile, "size", orig_newSize );
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
        LOG_ERROR( "Error deserializing event: %s", ex.what() );
    }

    if( isValid )
    {
        eventBuilder
            .WithUCID( orig_ucid )
            .WithType( orig_evtype )
            .WithPackageID( orig_packageName )
            .WithOldFile( orig_oldPath, orig_oldHash, orig_oldSize )
            .WithNewFile( orig_newPath, orig_newHash, orig_newSize )
            .WithError( orig_errCode, orig_errMessage )
            .WithSubError( orig_subErrCode, orig_subErrType )
            .WithFrom( orig_fromVersion )
            .WithTse( orig_tse );
    }

    return isValid;
}

bool CloudEventBuilder::operator==( const CloudEventBuilder& other ) const
{
    bool result = m_ucid._Equal( other.m_ucid ) &&
        m_evtype == other.m_evtype &&
        m_packageName._Equal( other.m_packageName ) &&
        m_packageVersion._Equal( other.m_packageVersion ) &&
        m_errCode == other.m_errCode &&
        m_errMessage._Equal( other.m_errMessage ) &&
        m_subErrCode == other.m_subErrCode &&
        m_subErrType._Equal( other.m_subErrType ) &&
        m_oldPath == other.m_oldPath &&
        m_oldHash._Equal( other.m_oldHash ) &&
        m_oldSize == other.m_oldSize &&
        m_newPath == other.m_newPath &&
        m_newHash._Equal( other.m_newHash ) &&
        m_newSize == other.m_newSize &&
        m_fromVersion == other.m_fromVersion;

    if( !result )
    {
        std::stringstream ss;
        ss <<
            "'" << m_ucid << "' ?= '" << other.m_ucid << "', " <<
            "'" << m_evtype << "' ?= '" << other.m_evtype << "', " <<
            "'" << m_packageName << "' ?= '" << other.m_packageName << "', " <<
            "'" << m_packageVersion << "' ?= '" << other.m_packageVersion << "', " <<
            "'" << m_errCode << "' ?= '" << other.m_errCode << "', " <<
            "'" << m_errMessage << "' ?= '" << other.m_errMessage << "', " <<
            "'" << m_subErrCode << "' ?= '" << other.m_subErrCode << "', " <<
            "'" << m_subErrType << "' ?= '" << other.m_subErrType << "', " <<
            "'" << m_oldPath << "' ?= '" << other.m_oldPath << "', " <<
            "'" << m_oldHash << "' ?= '" << other.m_oldHash << "', " <<
            "'" << m_oldSize << "' ?= '" << other.m_oldSize << "', " <<
            "'" << m_newPath << "' ?= '" << other.m_newPath << "', " <<
            "'" << m_newHash << "' ?= '" << other.m_newHash << "', " <<
            "'" << m_newSize << "' ?= '" << other.m_newSize << "', " <<
            "'" << m_fromVersion << "' ?= '" << other.m_fromVersion << "'";

        LOG_DEBUG( "Events not equal: %s", ss.str().c_str() );
    }

    return result;
}

#pragma region PRIVATE

void CloudEventBuilder::UpdateEventTime()
{
    m_tse = TimeUtil::Now_RFC3339();
}

std::string CloudEventBuilder::Serialize()
{
    Json::Value event;
    event[ "type" ] = CloudEventString( m_evtype );
    event[ "package" ] = m_packageName + "/" + m_packageVersion;

    if( m_evtype == pkgreconfig )
    {
        if( !m_oldPath.empty() )
        {
            Json::Value oldfilearr;
            Json::Value oldfile;
            oldfile[ "path" ] = m_oldPath.generic_u8string();

            if ( !m_oldHash.empty() )
            {
                oldfile[ "sha256" ] = m_oldHash;
            }

            oldfile[ "size" ] = m_oldSize;
            oldfilearr[ 0 ] = oldfile;
            event[ "old" ] = oldfilearr;
        }

        if( !m_newPath.empty() )
        {
            Json::Value newfilearr;
            Json::Value newfile;
            newfile[ "path" ] = m_newPath.generic_u8string();

            if ( !m_newHash.empty() )
            {
                newfile[ "sha256" ] = m_newHash;
            }

            newfile[ "size" ] = m_newSize;
            newfilearr[ 0 ] = newfile;
            event[ "new" ] = newfilearr;
        }
    }
    else if( m_evtype == pkginstall )
    {
        if( !m_fromVersion.empty() )
        {
            event[ "from" ] = m_fromVersion;
        }
    }

    if( m_errCode != 0 || m_subErrCode != 0 )
    {
        Json::Value error;
        error[ "code" ] = ( unsigned )m_errCode;
        error[ "msg" ] = m_errMessage;
        if( m_subErrCode != 0 )
        {
            error[ "sub_code" ] = ( unsigned )m_subErrCode;
            error[ "sub_type" ] = m_subErrType;
        }
        event[ "err" ] = error;
    }

    event[ "tse" ] = m_tse;
    event[ "tstx" ] = TimeUtil::Now_RFC3339();
    event[ "ucid" ] = m_ucid;

    Json::Value root;
    root[ "event" ] = event;

    return Json::writeString( Json::StreamWriterBuilder(), root );
}

#pragma endregion PRIVATE

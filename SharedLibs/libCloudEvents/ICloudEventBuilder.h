#pragma once

#include <string>

enum CloudEventType
{
    pkginstall = 0,
    pkgreconfig,
    pkguninstall
};

class ICloudEventBuilder
{
public:
    ICloudEventBuilder() {};
    virtual ~ICloudEventBuilder() = default;

    virtual ICloudEventBuilder& WithUCID( const std::string ucid ) = 0;
    virtual ICloudEventBuilder& WithType( CloudEventType type ) = 0;
    virtual ICloudEventBuilder& WithPackage( const std::string name, const std::string version ) = 0;
    virtual ICloudEventBuilder& WithError( int code, const std::string message ) = 0;
    virtual ICloudEventBuilder& WithOldFile( const std::string path, const std::string hash, int size ) = 0;
    virtual ICloudEventBuilder& WithNewFile( const std::string path, const std::string hash, int size ) = 0;
    virtual ICloudEventBuilder& WithJsonString( const std::string json ) = 0;
    virtual std::string Build() = 0;
    virtual void Reset() = 0;

    ICloudEventBuilder( ICloudEventBuilder&& rhs ) = default;
};

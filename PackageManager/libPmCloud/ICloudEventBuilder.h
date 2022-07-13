#pragma once

#include <string>
#include <filesystem>

enum CloudEventType
{
    pkgunknown,
    pkginstall,
    pkgreconfig,
    pkguninstall
};

class ICloudEventBuilder
{
public:
    ICloudEventBuilder() {};
    ICloudEventBuilder( ICloudEventBuilder&& rhs ) = default;
    virtual ~ICloudEventBuilder() {};

    virtual ICloudEventBuilder& FromJson( const std::string& eventJson ) = 0;
    virtual ICloudEventBuilder& WithUCID( const std::string& ucid ) = 0;
    virtual ICloudEventBuilder& WithType( CloudEventType evtype ) = 0;
    virtual ICloudEventBuilder& WithPackageID( const std::string& idAsNameAndVersion ) = 0; // e.g. 'AMP/1.0.0'
    virtual ICloudEventBuilder& WithPackage( const std::string& name, const std::string& version ) = 0;
    virtual ICloudEventBuilder& WithError( int code, const std::string& message ) = 0;
    virtual ICloudEventBuilder& WithSubError( int subErrCode, const std::string& subErrType ) = 0;
    virtual ICloudEventBuilder& WithOldFile( const std::filesystem::path& path, const std::string& hash, uint64_t size ) = 0;
    virtual ICloudEventBuilder& WithNewFile( const std::filesystem::path& path, const std::string& hash, uint64_t size ) = 0;
    virtual ICloudEventBuilder& WithFrom( const std::string& fromVersion ) = 0;
    virtual ICloudEventBuilder& WithTse( const std::string& tse ) = 0;
    virtual ICloudEventBuilder& WithTseNow() = 0;

    virtual std::string GetPackageName() const = 0;
    virtual std::string GetPackageVersion() const = 0;
    virtual std::string GetRFC3339Tse() const = 0;

    virtual std::string Build() = 0;
    virtual void Reset() = 0;
};

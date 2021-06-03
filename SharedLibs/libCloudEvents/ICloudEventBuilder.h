#pragma once

#include <string>

enum CloudEventType
{
    pkgunknown,
    pkginstall,
    pkgreconfig,
    pkguninstall,
    pkguninstallbegin,
    pkguninstallerror,
    pkguninstallcomplete
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
    virtual ICloudEventBuilder& WithOldFile( const std::string& path, const std::string& hash, int size ) = 0;
    virtual ICloudEventBuilder& WithNewFile( const std::string& path, const std::string& hash, int size ) = 0;

    virtual std::string GetPackageName() = 0;
    virtual std::string GetPackageVersion() = 0;

    virtual std::string Build() = 0;
    virtual void Reset() = 0;
};

#pragma once

#include "ICloudEventBuilder.h"
#include "json\json.h"

static std::string CloudEventString( CloudEventType eventType )
{
    switch( eventType )
    {
    case pkginstall: return "pkg-install";
    case pkgreconfig: return "pkg-reconfig";
    default: return "pkg-uninstall";
    }
}

static CloudEventType ConvertCloudEventType( const std::string& eventType )
{
    if ( eventType == "pkg-install" )
        return pkginstall;
    else if ( eventType == "pkg-reconfig" )
        return pkgreconfig;
    else if ( eventType == "pkg-uninstall" )
        return pkguninstall;
    else
        return pkgunknown;
}

class CloudEventBuilder final : public ICloudEventBuilder
{
public:
    CloudEventBuilder();
    ~CloudEventBuilder();
    
    ICloudEventBuilder& FromJson( const std::string& eventJson ) override;
    ICloudEventBuilder& WithUCID( const std::string& ucid ) override;
    ICloudEventBuilder& WithType( CloudEventType evtype ) override;
    ICloudEventBuilder& WithPackageID( const std::string& idAsNameAndVersion ) override; // e.g. 'AMP/1.0.0'
    ICloudEventBuilder& WithPackage( const std::string& name, const std::string& version ) override;
    ICloudEventBuilder& WithError( int code, const std::string& message ) override;
    ICloudEventBuilder& WithOldFile( const std::string& path, const std::string& hash, int size ) override;
    ICloudEventBuilder& WithNewFile( const std::string& path, const std::string& hash, int size ) override;

    std::string GetPackageName() override;
    std::string GetPackageVersion() override;

    std::string Build() override;
    void Reset() override;

    static bool Deserialize( ICloudEventBuilder& event, const std::string& eventJson );
    static bool ExtractJsonInt( Json::Value& root, const std::string& attribute, int& dest );
    static bool ExtractJsonString( Json::Value& root, const std::string& attribute, std::string& dest );

private:
    std::string m_ucid;
    CloudEventType m_evtype;
    std::string m_packageName;
    std::string m_packageVersion;
    int m_errCode;
    std::string m_errMessage;
    std::string m_oldPath;
    std::string m_oldHash;
    int m_oldSize;
    std::string m_newPath;
    std::string m_newHash;
    int m_newSize;
    std::string m_tse;

    std::string Now_RFC3339();
    void UpdateEventTime();
    std::string Serialize();
};

#pragma once

#include "ICloudEventBuilder.h"
#include "EnumStringConversion.h"

template<> char const* EnumStrings<CloudEventType>::data[] = {
    "pkg-install",
    "pkg-reconfig",
    "pkg-uninstall"
};

class CloudEventBuilder final : public ICloudEventBuilder
{
public:
    CloudEventBuilder();
    ~CloudEventBuilder();
    
    ICloudEventBuilder& WithUCID( const std::string ucid ) override;
    ICloudEventBuilder& WithType( CloudEventType evtype ) override;
    ICloudEventBuilder& WithPackage( const std::string name, const std::string version ) override;
    ICloudEventBuilder& WithError( int code, const std::string message ) override;
    ICloudEventBuilder& WithOldFile( const std::string path, const std::string hash, int size ) override;
    ICloudEventBuilder& WithNewFile( const std::string path, const std::string hash, int size ) override;
    std::string Build() override;
    void Reset() override;

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

    std::string Now_RFC3339();
};

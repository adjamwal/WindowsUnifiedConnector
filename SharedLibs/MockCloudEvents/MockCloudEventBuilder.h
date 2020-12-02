#pragma once

#include "pch.h"
#include "ICloudEventBuilder.h"

class MockCloudEventBuilder : public ICloudEventBuilder
{
public:
    MockCloudEventBuilder();
    ~MockCloudEventBuilder();

    MOCK_METHOD1( FromJson, ICloudEventBuilder& ( const std::string& ) );
    MOCK_METHOD1( WithUCID, ICloudEventBuilder& ( const std::string& ) );
    MOCK_METHOD1( WithType, ICloudEventBuilder& ( CloudEventType ) );
    MOCK_METHOD1( WithPackageID, ICloudEventBuilder& ( const std::string& ) );
    MOCK_METHOD2( WithPackage, ICloudEventBuilder& ( const std::string&, const std::string& ) );
    MOCK_METHOD2( WithError, ICloudEventBuilder& ( int, const std::string& ) );
    MOCK_METHOD3( WithOldFile, ICloudEventBuilder& ( const std::string&, const std::string&, int ) );
    MOCK_METHOD3( WithNewFile, ICloudEventBuilder& ( const std::string&, const std::string&, int ) );

    MOCK_METHOD0( GetPackageName, std::string() );
    MOCK_METHOD0( GetPackageVersion, std::string() );

    MOCK_METHOD0( Build, std::string() );
    void MakeBuildReturn( std::string value );
    void ExpectBuildIsNotCalled();

    MOCK_METHOD0( Reset, void() );
    void ExpectResetIsNotCalled();
};

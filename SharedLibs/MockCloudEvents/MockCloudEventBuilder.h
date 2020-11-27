#pragma once

#include "pch.h"
#include "ICloudEventBuilder.h"

class MockCloudEventBuilder : public ICloudEventBuilder
{
public:
    MockCloudEventBuilder();
    ~MockCloudEventBuilder();

    MOCK_METHOD0( Build, std::string() );
    void MakeBuildReturn( std::string value );
    void ExpectBuildNotCalled();

    MOCK_METHOD0( Reset, void() );
    void ExpectResetNotCalled();
};
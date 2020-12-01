#pragma once

#include "pch.h"
#include "ICloudEventStorage.h"
#include "ICloudEventBuilder.h"

class MockCloudEventStorage : public ICloudEventStorage
{
public:
    MockCloudEventStorage();
    ~MockCloudEventStorage();

    MOCK_METHOD1( SaveEvent, int32_t( ICloudEventBuilder& event ) );
    MOCK_METHOD1( SaveEvent, int32_t( const std::string& event ) );
    void MakeSaveEventReturn( int32_t value );
    void ExpectSaveEventNotCalled();

    MOCK_METHOD0( ReadEvents, std::vector<std::string>() );
    void MakeReadEventsReturn( std::vector<std::string> value );
    void ExpectReadEventsNotCalled();
};

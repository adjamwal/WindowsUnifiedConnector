#pragma once

#include "MocksCommon.h"
#include "ICloudEventStorage.h"
#include "ICloudEventBuilder.h"

class MockCloudEventStorage : public ICloudEventStorage
{
public:
    MockCloudEventStorage();
    ~MockCloudEventStorage();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* dep ) );
    MOCK_METHOD1( SaveEvent, bool( ICloudEventBuilder& event ) );
    MOCK_METHOD1( SaveEvent, bool( const std::string& event ) );
    void MakeSaveEventReturn( bool value );
    void ExpectSaveEventNotCalled();

    MOCK_METHOD0( ReadAndRemoveEvents, std::vector<std::string>() );
    void MakeReadAndRemoveEventsReturn( std::vector<std::string> value );
    void ExpectReadEventsNotCalled();
};

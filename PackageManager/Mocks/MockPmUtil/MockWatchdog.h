#pragma once
#include "MocksCommon.h"

#include "IWatchdog.h"

class MockWatchdog : public IWatchdog
{
public:
    MockWatchdog();
    ~MockWatchdog();

    MOCK_METHOD2( Start, void( std::function<std::chrono::milliseconds()>, std::function<void()> ) );
    void ExpectStartIsNotCalled();

    MOCK_METHOD0( Stop, void() );
    void ExpectStopIsNotCalled();

    MOCK_METHOD0( Kick, void() );
    void ExpectKickIsNotCalled();
};
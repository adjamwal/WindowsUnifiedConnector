#pragma once

#include "MocksCommon.h"
#include "IRebootHandler.h"

class MockRebootHandler : public IRebootHandler
{
public:
    MockRebootHandler();
    ~MockRebootHandler();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* ) );
    void ExpectInitializeNotCalled();

    MOCK_METHOD1( HandleReboot, bool( bool ) );
    void MakeHandleRebootReturn( bool value );
    void ExpectHandleRebootNotCalled();
};
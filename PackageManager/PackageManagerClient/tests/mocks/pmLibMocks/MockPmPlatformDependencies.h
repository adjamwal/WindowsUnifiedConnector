#pragma once
#include "MocksCommon.h"

#include "IPmPlatformDependencies.h"

class MockPmPlatformDependencies : public IPmPlatformDependencies
{
public:
    MockPmPlatformDependencies();
    ~MockPmPlatformDependencies();

    MOCK_METHOD0( Configuration, IPmPlatformConfiguration&() );
    void MakeConfigurationReturn( IPmPlatformConfiguration& value );
    void ExpectConfigurationIsNotCalled();

    MOCK_METHOD0( ComponentManager, IPmPlatformComponentManager&() );
    void MakeComponentManagerReturn( IPmPlatformComponentManager& value );
    void ExpectComponentManagerIsNotCalled();

};

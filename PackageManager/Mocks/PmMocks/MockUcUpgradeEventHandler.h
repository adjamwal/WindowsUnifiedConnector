#pragma once
#include "MocksCommon.h"

#include "IUcUpgradeEventHandler.h"

class MockUcUpgradeEventHandler: public IUcUpgradeEventHandler
{
public:
    MockUcUpgradeEventHandler();
    ~MockUcUpgradeEventHandler();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* ) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD1( StoreUcUpgradeEvent, bool( const std::string& ) );
    void MakeStoreUcUpgradeEventReturn( bool value );
    void ExpectStoreUcUpgradeEventIsNotCalled();

    MOCK_METHOD0( PublishUcUpgradeEvent, bool() );
    void MakePublishUcUpgradeEventReturn( bool value );
    void ExpectPublishUcUpgradeEventIsNotCalled();
};

#pragma once
#include "MocksCommon.h"

#include "IUcidAdapter.h"

class MockUcidAdapter : public IUcidAdapter
{
public:
    MockUcidAdapter();
    ~MockUcidAdapter();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* ) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD0( GetAccessToken, std::string() );
    void MakeGetUcidTokenReturn( std::string value );
    void ExpectGetUcidTokenIsNotCalled();

    MOCK_METHOD0( GetIdentity, std::string() );
    void MakeGetIdentityReturn( std::string value );
    void ExpectGetIdentityIsNotCalled();

    MOCK_METHOD0( Refresh, bool() );
    void MakeRefreshReturn( bool value );
    void ExpectRefreshIsNotCalled();
};

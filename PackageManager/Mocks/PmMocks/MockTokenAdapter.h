#pragma once
#include "MocksCommon.h"

#include "ITokenAdapter.h"

class MockTokenAdapter : public ITokenAdapter
{
public:
    MockTokenAdapter();
    ~MockTokenAdapter();

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

#include "MockUcidAdapter.h"

MockUcidAdapter::MockUcidAdapter()
{
    MakeGetUcidTokenReturn( std::string() );
}

MockUcidAdapter::~MockUcidAdapter()
{
}

void MockUcidAdapter::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockUcidAdapter::MakeGetUcidTokenReturn( std::string value )
{
    ON_CALL( *this, GetAccessToken() ).WillByDefault( Return( value ) );
}

void MockUcidAdapter::ExpectGetUcidTokenIsNotCalled()
{
    EXPECT_CALL( *this, GetAccessToken() ).Times( 0 );
}

void MockUcidAdapter::MakeGetIdentityReturn( std::string value )
{
    ON_CALL( *this, GetIdentity() ).WillByDefault( Return( value ) );
}

void MockUcidAdapter::ExpectGetIdentityIsNotCalled()
{
    EXPECT_CALL( *this, GetIdentity() ).Times( 0 );
}

void MockUcidAdapter::MakeRefreshReturn( bool value )
{
    ON_CALL( *this, Refresh() ).WillByDefault( Return( value ) );
}

void MockUcidAdapter::ExpectRefreshIsNotCalled()
{
    EXPECT_CALL( *this, Refresh() ).Times( 0 );
}

void MockUcidAdapter::MakeGetUrlsReturn( bool value )
{
    ON_CALL( *this, GetUrls( _ ) ).WillByDefault(Return(value));
}

void MockUcidAdapter::ExpectGetUrlsIsNotCalled()
{
    EXPECT_CALL( *this, GetUrls( _ ) ).Times( 0 );
}
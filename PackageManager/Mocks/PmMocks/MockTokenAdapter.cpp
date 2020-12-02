#include "MockTokenAdapter.h"

MockTokenAdapter::MockTokenAdapter()
{
    MakeGetUcidTokenReturn( std::string() );
}

MockTokenAdapter::~MockTokenAdapter()
{
}

void MockTokenAdapter::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockTokenAdapter::MakeGetUcidTokenReturn( std::string value )
{
    ON_CALL( *this, GetAccessToken() ).WillByDefault( Return( value ) );
}

void MockTokenAdapter::ExpectGetUcidTokenIsNotCalled()
{
    EXPECT_CALL( *this, GetAccessToken() ).Times( 0 );
}

void MockTokenAdapter::MakeGetIdentityReturn( std::string value )
{
    ON_CALL( *this, GetIdentity() ).WillByDefault( Return( value ) );
}

void MockTokenAdapter::ExpectGetIdentityIsNotCalled()
{
    EXPECT_CALL( *this, GetIdentity() ).Times( 0 );
}

void MockTokenAdapter::MakeRefreshReturn( bool value )
{
    ON_CALL( *this, Refresh() ).WillByDefault( Return( value ) );
}

void MockTokenAdapter::ExpectRefreshIsNotCalled()
{
    EXPECT_CALL( *this, Refresh() ).Times( 0 );
}
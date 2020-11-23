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
    ON_CALL( *this, GetUcidToken() ).WillByDefault( Return( value ) );
}

void MockTokenAdapter::ExpectGetUcidTokenIsNotCalled()
{
    EXPECT_CALL( *this, GetUcidToken() ).Times( 0 );
}


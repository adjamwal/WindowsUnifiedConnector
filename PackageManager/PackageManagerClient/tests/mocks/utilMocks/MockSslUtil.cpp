#include "MockSslUtil.h"

MockSslUtil::MockSslUtil()
{

}

MockSslUtil::~MockSslUtil()
{

}

void MockSslUtil::MakeDecodeBase64Return( int32_t value )
{
    ON_CALL( *this, DecodeBase64( _, _ ) ).WillByDefault( Return( value ) );
}

void MockSslUtil::ExpectDecodeBase64NotCalled()
{
    EXPECT_CALL( *this, DecodeBase64( _, _ ) ).Times( 0 );
}

void MockSslUtil::MakeCalculateSHA256Return( bool value )
{
    ON_CALL( *this, CalculateSHA256( _, _ ) ).WillByDefault( Return( value ) );
}

void MockSslUtil::ExpectCalculateSHA256NotCalled()
{
    EXPECT_CALL( *this, CalculateSHA256( _, _ ) ).Times( 0 );
}
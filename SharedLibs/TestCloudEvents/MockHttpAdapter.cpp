#include "pch.h"
#include "MockHttpAdapter.h"

MockHttpAdapter::MockHttpAdapter()
{
    MakeInitReturn( 0 );
    MakeDeinitReturn( 0 );
    MakeSetCertsReturn( 0 );
    MakeHttpGetReturn( 0 );
    MakeHttpPostReturn( 0 );
}

MockHttpAdapter::~MockHttpAdapter()
{
}

void MockHttpAdapter::MakeInitReturn( int32_t value )
{
    ON_CALL( *this, Init( _, _ ) ).WillByDefault( Return( value ) );
}

void MockHttpAdapter::ExpectInitIsNotCalled()
{
    EXPECT_CALL( *this, Init( _, _ ) ).Times( 0 );
}

void MockHttpAdapter::MakeDeinitReturn( int32_t value )
{
    ON_CALL( *this, Deinit() ).WillByDefault( Return( value ) );
}

void MockHttpAdapter::ExpectDeinitIsNotCalled()
{
    EXPECT_CALL( *this, Deinit() ).Times( 0 );
}

void MockHttpAdapter::MakeSetCertsReturn( int32_t value )
{
    ON_CALL( *this, SetCerts( _ ) ).WillByDefault( Return( value ) );
}

void MockHttpAdapter::ExpectSetCertsIsNotCalled()
{
    EXPECT_CALL( *this, SetCerts( _ ) ).Times( 0 );
}

void MockHttpAdapter::MakeHttpGetReturn( int32_t value )
{
    ON_CALL( *this, HttpGet( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockHttpAdapter::ExpectHttpGetIsNotCalled()
{
    EXPECT_CALL( *this, HttpGet( _, _, _ ) ).Times( 0 );
}

void MockHttpAdapter::MakeHttpPostReturn( int32_t value )
{
    ON_CALL( *this, HttpPost( _, _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockHttpAdapter::ExpectHttpPostIsNotCalled()
{
    EXPECT_CALL( *this, HttpPost( _, _, _, _, _ ) ).Times( 0 );
}


#include "MockPmHttp.h"

MockPmHttp::MockPmHttp()
{
    MakeInitReturn( int32_t() );
    MakeDeinitReturn( int32_t() );
    MakeSetTokenReturn( int32_t() );
    MakeSetCertsReturn( int32_t() );
    MakeHttpGetReturn( int32_t() );
    MakeHttpPostReturn( int32_t(), 200 );
    MakeHttpDownloadReturn( int32_t() );
}

MockPmHttp::~MockPmHttp()
{
}

void MockPmHttp::MakeInitReturn( int32_t value )
{
    ON_CALL( *this, Init( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockPmHttp::ExpectInitIsNotCalled()
{
    EXPECT_CALL( *this, Init( _, _, _ ) ).Times( 0 );
}

void MockPmHttp::MakeDeinitReturn( int32_t value )
{
    ON_CALL( *this, Deinit() ).WillByDefault( Return( value ) );
}

void MockPmHttp::ExpectDeinitIsNotCalled()
{
    EXPECT_CALL( *this, Deinit() ).Times( 0 );
}

void MockPmHttp::MakeSetTokenReturn( int32_t value )
{
    ON_CALL( *this, SetToken( _ ) ).WillByDefault( Return( value ) );
}

void MockPmHttp::ExpectSetTokenIsNotCalled()
{
    EXPECT_CALL( *this, SetToken( _ ) ).Times( 0 );
}

void MockPmHttp::MakeSetCertsReturn( int32_t value )
{
    ON_CALL( *this, SetCerts( _ ) ).WillByDefault( Return( value ) );
}

void MockPmHttp::ExpectSetCertsIsNotCalled()
{
    EXPECT_CALL( *this, SetCerts( _ ) ).Times( 0 );
}

void MockPmHttp::MakeHttpGetReturn( int32_t value )
{
    ON_CALL( *this, HttpGet( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockPmHttp::ExpectHttpGetIsNotCalled()
{
    EXPECT_CALL( *this, HttpGet( _, _, _ ) ).Times( 0 );
}

void MockPmHttp::MakeHttpPostReturn( int32_t value  )
{
    ON_CALL( *this, HttpPost( _, _, _, _, _ ) ).WillByDefault( DoAll( ::testing::SetArgReferee<4>( 200 ), Return( value ) ) );
}

void MockPmHttp::MakeHttpPostReturn( int32_t value, int32_t httpResponse )
{
    ON_CALL( *this, HttpPost( _, _, _, _, _ ) ).WillByDefault( DoAll( ::testing::SetArgReferee<4>( httpResponse ), Return( value ) ) );
}

void MockPmHttp::ExpectHttpPostIsNotCalled()
{
    EXPECT_CALL( *this, HttpPost( _, _, _, _, _ ) ).Times( 0 );
}

void MockPmHttp::MakeHttpDownloadReturn( int32_t value )
{
    ON_CALL( *this, HttpDownload( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockPmHttp::ExpectHttpDownloadIsNotCalled()
{
    EXPECT_CALL( *this, HttpDownload( _, _, _ ) ).Times( 0 );
}


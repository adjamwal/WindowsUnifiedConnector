#include "MockPmCloud.h"

MockPmCloud::MockPmCloud()
{
    MakeCheckinReturn( int32_t() );
    MakeDownloadFileReturn( int32_t() );
}

MockPmCloud::~MockPmCloud()
{
}

void MockPmCloud::ExpectSetUriIsNotCalled()
{
    EXPECT_CALL( *this, SetUri( _ ) ).Times( 0 );
}

void MockPmCloud::ExpectSetTokenIsNotCalled()
{
    EXPECT_CALL( *this, SetToken( _ ) ).Times( 0 );
}

void MockPmCloud::ExpectSetCertsIsNotCalled()
{
    EXPECT_CALL( *this, SetCerts( _ ) ).Times( 0 );
}

void MockPmCloud::ExpectSetUserAgentIsNotCalled()
{
    EXPECT_CALL( *this, SetUserAgent( _ ) ).Times( 0 );
}

void MockPmCloud::ExpectSetShutdownFuncIsNotCalled()
{
    EXPECT_CALL( *this, SetShutdownFunc( _ ) ).Times( 0 );
}

void MockPmCloud::MakeCheckinReturn( int32_t value )
{
    ON_CALL( *this, Checkin( _, _ ) ).WillByDefault( Return( value ) );
}

void MockPmCloud::ExpectCheckinIsNotCalled()
{
    EXPECT_CALL( *this, Checkin( _, _ ) ).Times( 0 );
}

void MockPmCloud::MakePostReturn( int32_t value )
{
    ON_CALL( *this, Post( _, _, _, _, _ ) ).WillByDefault( DoAll( ::testing::SetArgReferee<4>( 200 ), Return( value ) ) );
}

void MockPmCloud::MakePostReturn( int32_t value, int32_t httpResponse )
{
    ON_CALL( *this, Post( _, _, _, _, _ ) ).WillByDefault( DoAll( ::testing::SetArgReferee<4>( httpResponse ), Return( value ) ) );
}

void MockPmCloud::ExpectPostIsNotCalled()
{
    EXPECT_CALL( *this, Post( _, _, _, _, _ ) ).Times( 0 );
}

void MockPmCloud::MakeDownloadFileReturn( int32_t value )
{
    ON_CALL( *this, DownloadFile( _, _ ) ).WillByDefault( Return( value ) );
}

void MockPmCloud::ExpectDownloadFileIsNotCalled()
{
    EXPECT_CALL( *this, DownloadFile( _, _ ) ).Times( 0 );
}


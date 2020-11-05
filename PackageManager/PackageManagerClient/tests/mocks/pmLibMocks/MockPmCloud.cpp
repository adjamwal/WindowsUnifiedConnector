#include "MockPmCloud.h"

MockPmCloud::MockPmCloud()
{
    MakeCheckinReturn( int32_t() );
    MakeSendEventReturn( int32_t() );
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

void MockPmCloud::MakeCheckinReturn( int32_t value )
{
    ON_CALL( *this, Checkin( _, _ ) ).WillByDefault( Return( value ) );
}

void MockPmCloud::ExpectCheckinIsNotCalled()
{
    EXPECT_CALL( *this, Checkin( _, _ ) ).Times( 0 );
}

void MockPmCloud::MakeSendEventReturn( int32_t value )
{
    ON_CALL( *this, SendEvent( _ ) ).WillByDefault( Return( value ) );
}

void MockPmCloud::ExpectSendEventIsNotCalled()
{
    EXPECT_CALL( *this, SendEvent( _ ) ).Times( 0 );
}

void MockPmCloud::MakeDownloadFileReturn( int32_t value )
{
    ON_CALL( *this, DownloadFile( _, _ ) ).WillByDefault( Return( value ) );
}

void MockPmCloud::ExpectDownloadFileIsNotCalled()
{
    EXPECT_CALL( *this, DownloadFile( _, _ ) ).Times( 0 );
}


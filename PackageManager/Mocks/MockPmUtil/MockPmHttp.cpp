#include "MockPmHttp.h"

MockPmHttp::MockPmHttp()
{
    MakeInitReturn( bool(), {} );
    MakeDeinitReturn( bool() );
    MakeSetTokenReturn( bool(), {} );
    MakeSetCertsReturn( bool(), {} );
    MakeHttpGetReturn( bool(), "", { 200, 0 } );
    MakeHttpPostReturn( bool(), "", { 200, 0 } );
    MakeHttpDownloadReturn( bool(), { 200, 0 } );
}

MockPmHttp::~MockPmHttp()
{
}

void MockPmHttp::MakeInitReturn( bool retval, const PmHttpExtendedResult& eResult )
{
    ON_CALL( *this, Init( _, _, _, _ ) ).WillByDefault( DoAll(
        ::testing::SetArgReferee<3>( eResult ),
        Return( retval ) ) );
}

void MockPmHttp::ExpectInitIsNotCalled()
{
    EXPECT_CALL( *this, Init( _, _, _, _ ) ).Times( 0 );
}

void MockPmHttp::MakeDeinitReturn( bool retval )
{
    ON_CALL( *this, Deinit() ).WillByDefault( Return( retval ) );
}

void MockPmHttp::ExpectDeinitIsNotCalled()
{
    EXPECT_CALL( *this, Deinit() ).Times( 0 );
}

void MockPmHttp::MakeSetTokenReturn( bool retval, const PmHttpExtendedResult& eResult )
{
    ON_CALL( *this, SetToken( _, _ ) ).WillByDefault( DoAll(
        ::testing::SetArgReferee<1>( eResult ),
        Return( retval ) ) );
}

void MockPmHttp::ExpectSetTokenIsNotCalled()
{
    EXPECT_CALL( *this, SetToken( _, _ ) ).Times( 0 );
}

void MockPmHttp::MakeSetCertsReturn( bool retval, const PmHttpExtendedResult& eResult )
{
    ON_CALL( *this, SetCerts( _, _ ) ).WillByDefault( DoAll(
        ::testing::SetArgReferee<1>( eResult ),
        Return( retval ) ) );
}

void MockPmHttp::ExpectSetCertsIsNotCalled()
{
    EXPECT_CALL( *this, SetCerts( _, _ ) ).Times( 0 );
}

void MockPmHttp::MakeHttpGetReturn( bool retval, const std::string& responseContent, const PmHttpExtendedResult& eResult )
{
    ON_CALL( *this, HttpGet( _, _, _ ) ).WillByDefault( DoAll(
        ::testing::SetArgReferee<1>( responseContent ),
        ::testing::SetArgReferee<2>( eResult ),
        Return( retval ) ) );
}

void MockPmHttp::ExpectHttpGetIsNotCalled()
{
    EXPECT_CALL( *this, HttpGet( _, _, _ ) ).Times( 0 );
}

void MockPmHttp::MakeHttpPostReturn( bool retval, const std::string& responseContent, const PmHttpExtendedResult& eResult )
{
    ON_CALL( *this, HttpPost( _, _, _, _, _ ) ).WillByDefault( DoAll(
        ::testing::SetArgReferee<3>( responseContent ),
        ::testing::SetArgReferee<4>( eResult ),
        Return( retval ) ) );
}

void MockPmHttp::ExpectHttpPostIsNotCalled()
{
    EXPECT_CALL( *this, HttpPost( _, _, _, _, _ ) ).Times( 0 );
}

void MockPmHttp::MakeHttpDownloadReturn( bool retval, const PmHttpExtendedResult& eResult )
{
    ON_CALL( *this, HttpDownload( _, _, _ ) ).WillByDefault( DoAll(
        ::testing::SetArgReferee<2>( eResult ),
        Return( retval ) ) );
}

void MockPmHttp::ExpectHttpDownloadIsNotCalled()
{
    EXPECT_CALL( *this, HttpDownload( _, _, _ ) ).Times( 0 );
}

void MockPmHttp::MakeIsSslPeerValidationErrorReturn( bool retval )
{
    ON_CALL( *this, IsSslPeerValidationError( _ ) ).WillByDefault( Return( retval ) );
}

void MockPmHttp::ExpectIsSslPeerValidationErrorIsNotCalled()
{
    EXPECT_CALL( *this, IsSslPeerValidationError( _ ) ).Times( 0 );
}
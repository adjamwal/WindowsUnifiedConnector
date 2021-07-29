#include "MockPmCloud.h"

MockPmCloud::MockPmCloud()
{
    MakeCheckinReturn( bool(), "", {200, 0} );
    MakeGetReturn( bool(), "", { 200, 0 } );
    MakePostReturn( bool(), "", { 200, 0 } );
    MakeDownloadFileReturn( bool(), {200, 0} );
}

MockPmCloud::~MockPmCloud()
{
}

void MockPmCloud::ExpectSetCheckinUriIsNotCalled()
{
    EXPECT_CALL( *this, SetCheckinUri( _ ) ).Times( 0 );
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

void MockPmCloud::MakeCheckinReturn( bool retval, const std::string& responseContent, const PmHttpExtendedResult& eResult )
{
    ON_CALL( *this, Checkin( _, _, _ ) ).WillByDefault( DoAll( 
        ::testing::SetArgReferee<1>( responseContent ), 
        ::testing::SetArgReferee<2>( eResult ), 
        Return( retval ) ) );
}

void MockPmCloud::ExpectCheckinIsNotCalled()
{
    EXPECT_CALL( *this, Checkin( _, _, _ ) ).Times( 0 );
}

void MockPmCloud::MakeGetReturn( bool retval, const std::string& responseContent, const PmHttpExtendedResult& eResult )
{
    ON_CALL( *this, Get( _, _, _ ) ).WillByDefault( DoAll( 
        ::testing::SetArgReferee<1>( responseContent ), 
        ::testing::SetArgReferee<2>( eResult ), 
        Return( retval ) ) );
}

void MockPmCloud::ExpectGetIsNotCalled()
{
    EXPECT_CALL( *this, Get( _, _, _ ) ).Times( 0 );
}

void MockPmCloud::MakePostReturn( bool retval, const std::string& responseContent, const PmHttpExtendedResult& eResult )
{
    ON_CALL( *this, Post( _, _, _, _, _ ) ).WillByDefault( DoAll( 
        ::testing::SetArgReferee<3>( responseContent ),
        ::testing::SetArgReferee<4>( eResult ), 
        Return( retval ) ) );
}

void MockPmCloud::ExpectPostIsNotCalled()
{
    EXPECT_CALL( *this, Post( _, _, _, _, _ ) ).Times( 0 );
}

void MockPmCloud::MakeDownloadFileReturn( bool retval, const PmHttpExtendedResult& eResult )
{
    ON_CALL( *this, DownloadFile( _, _, _ ) ).WillByDefault( DoAll( 
        ::testing::SetArgReferee<2>( eResult ),
        Return( retval ) ) );
}

void MockPmCloud::ExpectDownloadFileIsNotCalled()
{
    EXPECT_CALL( *this, DownloadFile( _, _, _ ) ).Times( 0 );
}


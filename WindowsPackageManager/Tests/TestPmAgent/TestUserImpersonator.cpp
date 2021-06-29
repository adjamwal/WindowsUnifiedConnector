#include "pch.h"
#include "UserImpersonator.h"
#include "MockWinApiWrapper.h"
#include <memory>

class TestUserImpersonator : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_winApi.reset( new NiceMock<MockWinApiWrapper>() );

        m_patient.reset( new UserImpersonator( *m_winApi ) );
    }

    void TearDown()
    {
        m_patient.reset();

        m_winApi.reset();
    }

    std::unique_ptr<MockWinApiWrapper> m_winApi;

    std::unique_ptr<UserImpersonator> m_patient;
};

TEST_F( TestUserImpersonator, GetActiveUserSessionsWillSucceed )
{
    std::vector<ULONG> sessionList;

    m_winApi->MakeWTSEnumerateSessionsWReturn( TRUE );

    EXPECT_TRUE( m_patient->GetActiveUserSessions( sessionList ) );
}

TEST_F( TestUserImpersonator, GetActiveUserSessionsWillFail )
{
    std::vector<ULONG> sessionList;

    m_winApi->MakeWTSEnumerateSessionsWReturn( FALSE );

    EXPECT_FALSE( m_patient->GetActiveUserSessions( sessionList ) );
}

TEST_F( TestUserImpersonator, GetActiveUserSessionsWillFreeMemory )
{
    std::vector<ULONG> sessionList;

    ON_CALL( *m_winApi, WTSEnumerateSessions( _, _, _, _, _ ) ).WillByDefault( DoAll( 
        SetArgPointee<3>( ( PWTS_SESSION_INFO )1 ), 
        Return( TRUE ) ) );

    EXPECT_CALL( *m_winApi, WTSFreeMemory( _ ) );

    m_patient->GetActiveUserSessions( sessionList );
}

TEST_F( TestUserImpersonator, GetActiveUserSessionsGetActiveSessions )
{
    std::vector<ULONG> sessionList;
    PWTS_SESSION_INFO sessionInfo = ( PWTS_SESSION_INFO )malloc( sizeof( WTS_SESSION_INFO ) * 3 );
    ASSERT_NE( sessionInfo, ( PWTS_SESSION_INFO )NULL );

    for( int i = 0; i < 3; i++ ) {
        sessionInfo[ i ].SessionId = i;
        sessionInfo[ i ].State = ( ( i % 2 ) == 0 ) ? WTSActive : WTSIdle;
    }

    ON_CALL( *m_winApi, WTSEnumerateSessions( _, _, _, _, _ ) ).WillByDefault( DoAll(
        SetArgPointee<3>( sessionInfo ),
        SetArgPointee<4>( 3 ),
        Return( TRUE ) ) );

    m_patient->GetActiveUserSessions( sessionList );

    free( sessionInfo );

    ASSERT_EQ( sessionList.size(), 2 );
    EXPECT_EQ( sessionList[ 0 ], 0 );
    EXPECT_EQ( sessionList[ 1 ], 2 );
}

TEST_F( TestUserImpersonator, RunProcessInSessionWillSucceed )
{
    std::vector<ULONG> sessionList;

    ON_CALL( *m_winApi, WTSQueryUserToken( _, _ ) ).WillByDefault( DoAll( 
        SetArgPointee<1>( (HANDLE)1 ),
        Return( TRUE ) 
    ) );
    m_winApi->MakeCreateEnvironmentBlockReturn( TRUE );
    m_winApi->MakeCreateProcessAsUserWReturn( TRUE );

    EXPECT_TRUE( m_patient->RunProcessInSession( L"Process", L"Args", L"Directory", 0 ) );
}

TEST_F( TestUserImpersonator, RunProcessInSessionWillCreateProcess )
{
    std::vector<ULONG> sessionList;
    std::wstring process = L"Process";
    std::wstring args = L"Args";
    std::wstring dir = L"Directory";

    ON_CALL( *m_winApi, WTSQueryUserToken( _, _ ) ).WillByDefault( DoAll(
        SetArgPointee<1>( ( HANDLE )1 ),
        Return( TRUE )
    ) );
    m_winApi->MakeCreateEnvironmentBlockReturn( TRUE );

    EXPECT_CALL( *m_winApi, CreateProcessAsUserW( _, HasSubstr( process ), HasSubstr( args ), _, _, StrEq( dir ), _, _ ) );

    m_patient->RunProcessInSession( process, args, dir, 0 );
}

TEST_F( TestUserImpersonator, RunProcessInSessionWillDestroyEnvironmentBlock )
{
    std::vector<ULONG> sessionList;

    ON_CALL( *m_winApi, WTSQueryUserToken( _, _ ) ).WillByDefault( DoAll(
        SetArgPointee<1>( ( HANDLE )1 ),
        Return( TRUE )
    ) );
    ON_CALL( *m_winApi, CreateEnvironmentBlock( _, _, _ ) ).WillByDefault( DoAll(
        SetArgPointee<0>( ( LPVOID )2 ),
        Return( TRUE )
    ) );

    EXPECT_CALL( *m_winApi, DestroyEnvironmentBlock( ( LPVOID )2 ) );

    m_patient->RunProcessInSession( L"Process", L"Args", L"Directory", 0 );
}

TEST_F( TestUserImpersonator, RunProcessInSessionWillFailWithoutUserToken )
{
    std::vector<ULONG> sessionList;

    m_winApi->MakeWTSQueryUserTokenReturn( FALSE );

    EXPECT_FALSE( m_patient->RunProcessInSession( L"Process", L"Args", L"Directory", 0 ) );
}

TEST_F( TestUserImpersonator, RunProcessInSessionWillNotCreateProcessWithoutToken )
{
    std::vector<ULONG> sessionList;

    m_winApi->MakeWTSQueryUserTokenReturn( FALSE );

    m_winApi->ExpectCreateEnvironmentBlockNotCalled();
    m_winApi->ExpectCreateProcessAsUserWNotCalled();

    m_patient->RunProcessInSession( L"Process", L"Args", L"Directory", 0 );
}

TEST_F( TestUserImpersonator, RunProcessInSessionWillCloseUserToken )
{
    std::vector<ULONG> sessionList;

    ON_CALL( *m_winApi, WTSQueryUserToken( _, _ ) ).WillByDefault( DoAll(
        SetArgPointee<1>( ( HANDLE )1 ),
        Return( TRUE )
    ) );

    EXPECT_CALL( *m_winApi, CloseHandle( ( HANDLE )1 ) );

    m_patient->RunProcessInSession( L"Process", L"Args", L"Directory", 0 );
}

TEST_F( TestUserImpersonator, RunProcessInSessionWillNotCreateProcessWithoutEnvironment )
{
    std::vector<ULONG> sessionList;

    ON_CALL( *m_winApi, WTSQueryUserToken( _, _ ) ).WillByDefault( DoAll(
        SetArgPointee<1>( ( HANDLE )1 ),
        Return( TRUE )
    ) );
    m_winApi->MakeCreateEnvironmentBlockReturn( FALSE );

    m_winApi->ExpectCreateProcessAsUserWNotCalled();

    m_patient->RunProcessInSession( L"Process", L"Args", L"Directory", 0 );
}

TEST_F( TestUserImpersonator, RunProcessInSessionWillCloseProcessHandles )
{
    std::vector<ULONG> sessionList;
    PROCESS_INFORMATION processInfo = { 0 };
    processInfo.hProcess = ( HANDLE )2;
    processInfo.hThread = ( HANDLE )3;

    ON_CALL( *m_winApi, WTSQueryUserToken( _, _ ) ).WillByDefault( DoAll(
        SetArgPointee<1>( ( HANDLE )1 ),
        Return( TRUE )
    ) );
    m_winApi->MakeCreateEnvironmentBlockReturn( TRUE );
    ON_CALL( *m_winApi, CreateProcessAsUserW( _, _, _, _, _, _, _, _ ) ).WillByDefault( DoAll(
        SetArgPointee<7>( processInfo ),
        Return( TRUE )
    ) );

    InSequence S;
    EXPECT_CALL( *m_winApi, CloseHandle( ( HANDLE )3 ) );
    EXPECT_CALL( *m_winApi, CloseHandle( ( HANDLE )2 ) );
    EXPECT_CALL( *m_winApi, CloseHandle( ( HANDLE )1 ) );

    m_patient->RunProcessInSession( L"Process", L"Args", L"Directory", 0 );
}
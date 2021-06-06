#include "pch.h"
#include "UserImpersonator.h"
#include "WindowsUtilities.h"
#include "IUcLogger.h"
#include "IWinApiWrapper.h"

UserImpersonator::UserImpersonator( IWinApiWrapper& winApiWrapper ) :
    m_winApi( winApiWrapper )
{
}

UserImpersonator::~UserImpersonator()
{
}

bool UserImpersonator::GetActiveUserSessions( std::vector<ULONG>& sessionList )
{
    PWTS_SESSION_INFO pSessionInfo = NULL;
    DWORD sessionCount = 0;
    bool ret = false;

    sessionList.clear();

    ret = EnumerateSessions( WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &sessionCount );

    if( !ret ) {
        LOG_ERROR( "EnumerateSessions failed" );
    }
    else if( !pSessionInfo || sessionCount == 0 ) {
        LOG_DEBUG( "User Sessions not available" );
    }
    else {
        for( DWORD i = 0; i < sessionCount; i++ ) {
            DWORD sid = pSessionInfo[ i ].SessionId;
            if( pSessionInfo[ i ].State == WTSActive ) {
                sessionList.push_back( sid );
            }
        }

        LOG_DEBUG( "Found %d active sessions", sessionList.size() );
    }

    if( pSessionInfo ) {
        m_winApi.WTSFreeMemory( pSessionInfo );
    }

    return ret;
}

bool UserImpersonator::RunProcessInSession( const std::wstring& processName, 
    const std::wstring& args, 
    const std::wstring& workingDir, 
    ULONG sessionId )
{
    bool rtn = false;
    HANDLE userToken = NULL;
    
    if( !m_winApi.WTSQueryUserToken( sessionId, &userToken ) ) {
        LOG_ERROR( "WTSQueryUserToken failed for session %d", sessionId );
    }
    else if( userToken == NULL ) {
        LOG_ERROR( "userToken is NULL" );
    }
    else {
        rtn = RunProcessAs( userToken, processName, args, workingDir );
        m_winApi.CloseHandle( userToken );
    }

    return rtn;
}

bool UserImpersonator::RunProcessAs( HANDLE userToken, 
    const std::wstring& processName, 
    const std::wstring& args, 
    const std::wstring& workingDir )
{
    LOG_DEBUG( "Enter" );
    bool rtn = false;

    PROCESS_INFORMATION processInfo = { 0 };
    STARTUPINFO startupInfo = { 0 };
    std::wstring processArgs = processName + L" " + args;
    std::wstring fullProcessPath = workingDir;

    if( !fullProcessPath.empty() && fullProcessPath.back() != '\\' ) {
        fullProcessPath += '\\';
    }
    fullProcessPath += processName;

    startupInfo.cb = sizeof( STARTUPINFO );
    startupInfo.lpDesktop = ( LPTSTR )L"WinSta0\\default";
    startupInfo.dwFlags = STARTF_FORCEOFFFEEDBACK | STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_HIDE;

    if( userToken == NULL ) {
        LOG_ERROR( "userToken is NULL" );
        return rtn;
    }

    LPVOID pEnv = NULL;
    DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;

    rtn = m_winApi.CreateEnvironmentBlock( &pEnv, userToken, FALSE );

    if( !rtn ) {
        LOG_ERROR( "CreateEnvironmentBlock failed" );
    }
    else {
        dwCreationFlag |= CREATE_UNICODE_ENVIRONMENT;

        WLOG_DEBUG( L"Starting Process %s args %s", fullProcessPath.c_str(), processArgs.c_str() );
        rtn = m_winApi.CreateProcessAsUserW( userToken, 
            fullProcessPath.c_str(),
            ( LPTSTR )processArgs.c_str(),
            dwCreationFlag,
            pEnv,
            workingDir.c_str(),
            &startupInfo,
            &processInfo );

        if( !rtn ) {
            LOG_ERROR( "CreateProcessAsUser failed" );
        }
        else {
            WLOG_DEBUG( L"Created Process %s", processArgs.c_str() );
            m_winApi.CloseHandle( processInfo.hThread );
            m_winApi.CloseHandle( processInfo.hProcess );
        }
    }

    if( pEnv ) {
        m_winApi.DestroyEnvironmentBlock( pEnv );
        pEnv = NULL;
    }

    return rtn;
}

bool UserImpersonator::EnumerateSessions(
    HANDLE hserver,
    DWORD reserved,
    DWORD version,
    PWTS_SESSION_INFO* ppSessionInfo,
    DWORD* pCount )
{
    bool ret = false;
    if( ppSessionInfo && pCount ) {
        PWTS_SESSION_INFO pSessionInfo( NULL );
        DWORD count( 0 );


        ret = m_winApi.WTSEnumerateSessions( hserver, 0, 1, &pSessionInfo, &count ) ? true : false;

        if( ret ) {
            *ppSessionInfo = pSessionInfo;
            LOG_DEBUG( "WTSEnumerateSessions returned %d sessions", count );
        }
        else {
            LOG_ERROR( "WTSEnumerateSessions failed" );
        }

        *pCount = count;
    }
    else {
        LOG_ERROR( "invalid parameters" );
    }

    return ret;
}

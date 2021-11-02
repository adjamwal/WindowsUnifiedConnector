#include "ProxyStringParser.h"
#include <vector>
#include "ProxyInfoModel.h"
#include "IUcLogger.h"

typedef std::vector<std::wstring> STRING_LIST;

typedef enum _PROXY_PARSE_STATE
{
    INIT,
    L_SCHEME,
    R_SCHEME,
    PORT,
    FINAL,
    INVALID
}PROXY_PARSE_STATE;

WCHAR token_list[] = { '=',':', '/', '\0' };

DWORD state_transition[ 6 ][ 6 ] = { {L_SCHEME, PORT, INVALID, FINAL},
                 {INVALID, PORT, INVALID, FINAL},
                 {INVALID, PORT, INVALID, FINAL},
                 {INVALID, PORT, INVALID, FINAL},
                 {INVALID, INVALID, INVALID, INVALID},
                 {INVALID, INVALID, INVALID, INVALID},
};

int ProxyStringParser::GetToken( LPCTSTR buffer, DWORD& pos )
{
    if( !buffer ) return -1;
    pos = 0;

    for( int j = 0; j < sizeof( token_list ) / sizeof( WCHAR ); j++ ) {
        WCHAR* p = (WCHAR* )wcschr( buffer, token_list[ j ] );
        if( p ) {
            pos = (DWORD )( p - buffer );
            return j;
        }
    }

    return -1;
}

int ProxyStringParser::GetToken( LPCTSTR buffer, WCHAR c )
{
    if( !buffer ) return -1;

    for( int i = 0; buffer[ i ] != '\0'; i++ )
        if( buffer[ i ] == c ) return i;

    return -1;
}

DWORD ProxyStringParser::ProcessState( LPCTSTR buffer, DWORD& begin, DWORD& end, ProxyInfoModel* info )
{
    DWORD status = 1;
    if( !buffer || !info ) return 0;

    WCHAR temp[ 2048 ];

    state = state_transition[ prevState ][ tokenId ];
    wcsncpy_s( temp, sizeof( temp ) / sizeof( WCHAR ), &buffer[ begin ], end - begin );

    switch( state ) {
    case INIT:
        break;

    case L_SCHEME:
        info->SetProxyAccessType( temp );
        break;

    case R_SCHEME:
        info->SetProxyAccessType( temp );
        break;

    case PORT:
    {
        int token_id1, token_id2;
        ULONG len = (ULONG )wcslen( buffer );

        if( end + 2 < len ) {
            token_id1 = GetToken( &buffer[ end + 1 ], '/' );
            token_id2 = GetToken( &buffer[ end + 2 ], '/' );
            if( token_id1 != -1 ) {
                if( token_id2 == -1 ) {
                    status = 0;
                    goto abort;
                }
                end += 2;
                state = R_SCHEME;
                info->SetProxyType( temp );
            }
            else
                info->SetProxyServer( temp );
        }
        else	//In the rare event that port is a single digit
            info->SetProxyServer( temp );

        break;
    }

    case FINAL:
        if( prevState == PORT )
            info->SetProxyPort( _wtoi( temp ) );

        if( prevState == L_SCHEME || prevState == R_SCHEME || prevState == INIT )
            info->SetProxyServer( temp );

        status = 2;

        break;

    default:
        LOG_DEBUG( __FUNCTION__ " reached invalid state: %d, prev: %d", state, prevState );
        status = 0;
        goto abort;
    }

abort:
    prevState = state;
    return status;
}

#define PRE_PROCESS(string, ch, process) {\
	int t = 0; \
	while (1) \
	{ \
		t = GetToken(string, ch); \
		if (t == -1)  \
			break; \
		string[t] = '\0'; \
		process.push_back(string);  \
		t++; \
		string += t; \
	} \
}

BOOL ProxyStringParser::ParseProxyString( LPCTSTR proxyString, PROXY_INFO_LIST& info, DWORD discoveryMode )
{
    BOOL status = FALSE;
    DWORD pos = 0, prev = 0, len = 0, end = 0;

    if( !proxyString ) return FALSE;
    WCHAR* pTemp = _wcsdup( proxyString ), * pTempString = NULL;
    if( !pTemp ) return FALSE;
    pTempString = pTemp;


    STRING_LIST process;
    PRE_PROCESS( pTemp, ' ', process );
    PRE_PROCESS( pTemp, ';', process );
    process.push_back( pTemp );

    if( process.size() == 0 )
        process.push_back( pTemp );

    for( size_t i = 0; i < process.size(); i++ ) {
        const WCHAR* pString = process.at( i ).c_str();
        pos = 0, prev = 0, len = 0, end = 0;

        len = (DWORD )wcslen( pString );
        state = prevState = INIT;
        tokenId = -1;
        ProxyInfoModel serverInfo;

        while( 1 ) {
            prev = end;
            if( end >= len ) {
                LOG_DEBUG( __FUNCTION__ " pos: %d, len: %d", pos, len );
                break;
            }

            tokenId = GetToken( &pString[ prev ], pos );
            end = prev + pos;
            if( !ProcessState( pString, prev, end, &serverInfo ) ) {
                LOG_DEBUG( __FUNCTION__ " prev: %d, pos: %d", prev, pos );
                break;
            }

            if( state == FINAL ) {
                serverInfo.SetProxyDiscoveryMode( discoveryMode );
                status = TRUE;
                break;
            }
            end++;
        }
        info.push_back( serverInfo );
        WLOG_DEBUG( __FUNCTION__ L": Add Proxy %s:%d to discovery list",
            serverInfo.GetProxyServer().c_str(),
            serverInfo.GetProxyPort() );
    }

    if( pTempString ) free( pTempString );
    return status;
}

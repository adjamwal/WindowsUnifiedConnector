#pragma once

#include "MocksCommon.h"
#include "IWinHttpWrapper.h"

class MockWinHttpWrapper : public IWinHttpWrapper
{
public:
    MockWinHttpWrapper();
    virtual ~MockWinHttpWrapper();

    MOCK_METHOD1( WinHttpGetDefaultProxyConfiguration, BOOL( WINHTTP_PROXY_INFO* ) );
    MOCK_METHOD1( WinHttpGetIEProxyConfigForCurrentUser, BOOL( WINHTTP_CURRENT_USER_IE_PROXY_CONFIG* ) );
    MOCK_METHOD5( WinHttpOpen, HINTERNET( LPCWSTR,
        DWORD,
        LPCWSTR,
        LPCWSTR,
        DWORD ) );
    MOCK_METHOD4( WinHttpGetProxyForUrl, BOOL( HINTERNET,
        LPCWSTR,
        WINHTTP_AUTOPROXY_OPTIONS*,
        WINHTTP_PROXY_INFO* ) );
    MOCK_METHOD1( WinHttpCloseHandle, BOOL( HINTERNET ) );
};
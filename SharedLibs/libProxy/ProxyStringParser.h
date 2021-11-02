#pragma once
#include <windows.h>
#include <string>
#include "ProxyInfoModel.h"

class ProxyStringParser
{
protected:
    int state, prevState, tokenId;

    int GetToken( LPCTSTR buffer, DWORD& pos );
    int GetToken( LPCTSTR buffer, WCHAR c );
    DWORD ProcessState( LPCTSTR buffer, DWORD& begin, DWORD& end, ProxyInfoModel* info );

public:
    BOOL ParseProxyString( LPCTSTR proxyString, PROXY_INFO_LIST& info, DWORD discoveryMode );
};

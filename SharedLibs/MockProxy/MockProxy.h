#pragma once

#include "MocksCommon.h"
#include "IProxy.h"

class MockProxy : public IProxy
{
public:
    MockProxy();
    virtual ~MockProxy();

    MOCK_METHOD1( GetProxyInfo, BOOL( PROXY_INFO_LIST* ) );
    MOCK_METHOD3( Init, int( LPCTSTR, LPCTSTR, CancelProxyDiscoveryCb ) );
};

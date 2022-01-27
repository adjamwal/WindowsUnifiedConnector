#pragma once
#include "MocksCommon.h"

#include "IProxyVerifier.h"
#include "ProxyInfoModel.h"

class MockProxyVerifier : public IProxyVerifier
{
public:
    MockProxyVerifier();
    virtual ~MockProxyVerifier();

    MOCK_METHOD0( IsBusy, bool( ) );
    MOCK_METHOD1( IsValidProxy, bool( const ProxyInfoModel& ) );
};


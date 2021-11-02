#pragma once
#include "MocksCommon.h"

#include "IProxyConsumer.h"
#include "ProxyInfoModel.h"

class MockProxyConsumer : public IProxyConsumer
{
public:
    MockProxyConsumer();
    virtual ~MockProxyConsumer();

    MOCK_METHOD1( ProxiesDiscovered, void( const std::list<ProxyInfoModel>& ) );
};


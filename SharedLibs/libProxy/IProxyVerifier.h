#pragma once

class ProxyInfoModel;

class IProxyVerifier
{
public:
    IProxyVerifier() {}
    virtual ~IProxyVerifier() {}

    virtual bool IsBusy() = 0;
    virtual bool IsValidProxy( const ProxyInfoModel& testProxy ) = 0;
};

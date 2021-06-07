#pragma once

class IPmPlatformDependencies;

class IRebootHandler
{
public:
    virtual ~IRebootHandler() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual bool HandleReboot( bool rebootRequired ) = 0;
};

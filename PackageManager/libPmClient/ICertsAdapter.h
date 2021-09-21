#pragma once

#include <string>

class IPmPlatformDependencies;
struct PmHttpCertList;

class ICertsAdapter
{
public:
    ICertsAdapter() {}
    virtual ~ICertsAdapter() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual PmHttpCertList GetCertsList() = 0;
    virtual void ReloadCerts() = 0;
};

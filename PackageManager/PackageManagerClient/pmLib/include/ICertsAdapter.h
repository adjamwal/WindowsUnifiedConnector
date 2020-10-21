#pragma once

#include <string>

class IPmPlatformDependencies;
struct PmHttpCertList;

//boundary separation from the external IPmPlatformDependencies->Configuration.GetSslCertificates() and ReleaseSslCertificates() for proper mocking
class ICertsAdapter
{
public:
    ICertsAdapter() {}
    virtual ~ICertsAdapter() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual PmHttpCertList GetCertsList() = 0;
};

#pragma once

#include <openssl/ssl.h>

class IWinCertLoader
{
public:
    IWinCertLoader() {}
    virtual ~IWinCertLoader() {}

    virtual int LoadSystemCerts() = 0;
    virtual int UnloadSystemCerts() = 0;
    virtual int GetSystemCerts( X509*** trusted_certificates, size_t& cert_count ) = 0;
    virtual int FreeSystemCerts( X509** trusted_certificates, size_t cert_count ) = 0;
};

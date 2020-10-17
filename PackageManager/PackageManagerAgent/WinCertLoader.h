#pragma once

#include "IWinCertLoader.h"
#include <mutex>

class WinCertLoader : public IWinCertLoader
{
public:
    WinCertLoader();
    ~WinCertLoader();

    int LoadSystemCerts() override;
    int UnloadSystemCerts() override;
    int GetSystemCerts( X509*** trusted_certificates, size_t& cert_count ) override;
    int FreeSystemCerts( X509** trusted_certificates, size_t cert_count ) override;

private:
    std::mutex m_mutex;
    X509** m_certificates;
    size_t m_certcount;

    void FreeCerts( X509** trusted_certificates, size_t cert_count );
};

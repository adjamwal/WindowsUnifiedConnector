#pragma once

#include "IPmPlatformConfiguration.h"
#include "IUcLogger.h"
#include "UCIDApiDll.h"

class IWinCertLoader;

class WindowsConfiguration : public IPmPlatformConfiguration
{
public:
    WindowsConfiguration(IWinCertLoader& winCertLoader, ICodesignVerifier& codeSignVerifier);
    ~WindowsConfiguration();

    /**
     * @brief Retrieves the clients identity token. This token is used to identifcation/authentication when
     *   communicating with the cloud.
     */
    bool GetIdentityToken( std::string& token ) override;

    /**
     * @brief (Optional) Retrieves the clients system certs
     *   Needed in Windows since curl can't load system certs without schannel
     *
     *  @param[in|out] certificates - Array of certs returned. The platfrom should allocated these
     *  @param[out] certificates - Number to certs returned
     */
    int32_t GetSslCertificates( X509*** certificates, size_t& count ) override;

    /**
     * @brief (Optional) Frees the cert list allocated by GetSslCertificates
     *
     *  @param[in] certificates - The cert array to be freed
     *  @param[in] certificates - Number to certs in the array
     */
    void ReleaseSslCertificates( X509** certificates, size_t count ) override;

    /**
     * @brief Provides the user agent for http requests
     */
    std::string GetHttpUserAgent() override;

private:
    IWinCertLoader& m_winCertLoader;
    UCIDApiDll m_ucidApi;
};
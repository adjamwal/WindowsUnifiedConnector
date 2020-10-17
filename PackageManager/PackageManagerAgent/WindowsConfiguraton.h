#pragma once

#include "IPmPlatformConfiguration.h"

class IWinCertLoader;

class WindowsConfiguration : public IPmPlatformConfiguration
{
public:
    WindowsConfiguration( IWinCertLoader& winCertLoader );
    ~WindowsConfiguration();

    /**
     * @brief Retrieves the clients identity token. This token is used to identifcation/authentication when
     *   communicating with the cloud.
     */
    int32_t GetIdentity( char* token, size_t& tokenLength ) override
    {
        return -1;
    }

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

private:
    IWinCertLoader& m_winCertLoader;
};
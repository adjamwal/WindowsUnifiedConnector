#pragma once

#include <cstdint>
#include <string>
#include <openssl/ssl.h>
#include "PmTypes.h"

/**
 * @file IPmPlatformConfiguration.h
 *
 * @brief This contains the package manager configuration interface
 */
class IPmPlatformConfiguration
{
public:
    IPmPlatformConfiguration() {}
    virtual ~IPmPlatformConfiguration() {}

    /**
     * @brief Retrieves the clients identity token. This token is used to identifcation/authentication when
     *   communicating with the cloud. This may return a cached value
     */
    virtual bool GetIdentityToken( std::string& token ) = 0;

    /**
     * @brief Retrieves the clients identity token. This token is used to identifcation/authentication when
     *   communicating with the cloud. This may return a cached value
     */
    virtual bool GetUcIdentity(std::string& identity) = 0;

    /**
     * @brief Refreshes the UCID and UCID token values
     */
    virtual bool RefreshIdentity() = 0;

    /**
     * @brief (Optional) Retrieves the clients system certs
     *   Needed in Windows since curl can't load system certs without schannel
     *
     *  @param[in|out] certificates - Array of certs returned. The platfrom should allocated these
     *  @param[out] certificates - Number to certs returned
     */
    virtual int32_t GetSslCertificates( X509*** certificates, size_t &count ) = 0;

    /**
     * @brief (Optional) Frees the cert list allocated by GetSslCertificates
     *
     *  @param[in] certificates - The cert array to be freed
     *  @param[in] certificates - Number to certs in the array
     */
    virtual void ReleaseSslCertificates( X509** certificates, size_t count ) = 0;

    /**
     * @brief Provides the user agent for http requests
     */
    virtual std::string GetHttpUserAgent() = 0;

    /**
     * @brief Gets the install directory
     */
    virtual std::string GetInstallDirectory() = 0;

    /**
     * @brief Gets the data directory
     */
    virtual std::string GetDataDirectory() = 0;

    /**
     * @brief Gets the PM version string
     */
    virtual std::string GetPmVersion() = 0;

    /**
     * @brief Retrieve the PM URLs from the identity module
     */
    virtual bool GetPmUrls( PmUrlList& urls ) = 0;
};
#pragma once

#include <cstdint>
#include <string>
#include <openssl/ssl.h>

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
     * TO be deleted? Not sure this is required
     */
    virtual int32_t GetConfigFileLocation( char* filename, size_t& filenameLength ) = 0;

    /**
     * @brief Retrieves the clients identity token. This token is used to identifcation/authentication when
     *   communicating with the cloud.
     */
    virtual bool GetIdentityToken( std::string& token ) = 0;

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
};
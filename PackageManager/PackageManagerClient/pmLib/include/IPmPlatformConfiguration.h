#pragma once

#include <cstdint>
#include <string>

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
     * @brief Load the UCID API.
     */
    virtual bool LoadUcidApi() = 0;

    /**
     * @brief Unload the UCID API.
     */
    virtual void UnloadUcidApi() = 0;

    /**
     * @brief Retrieves the clients identity id.
     */
    virtual bool GetIdentity( std::string& token ) = 0;

    /**
     * @brief Retrieves the clients identity token. This token is used to identifcation/authentication when
     *   communicating with the cloud.
     */
    virtual bool GetIdentityToken( std::string& token ) = 0;

    /**
     * @brief Retrieves the clients identity token. This token is used to identifcation/authentication when
     *   communicating with the cloud.
     */
    virtual bool RefreshIdentityToken() = 0;
};
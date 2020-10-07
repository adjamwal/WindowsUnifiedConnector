#pragma once

#include <cstdint>

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
    virtual int32_t GetIdentity( char* token, size_t& tokenLength ) = 0;
};
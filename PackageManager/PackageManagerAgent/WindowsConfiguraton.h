#pragma once

#include "IPmPlatformConfiguration.h"

class WindowsConfiguration : public IPmPlatformConfiguration
{
public:
    WindowsConfiguration() {}
    ~WindowsConfiguration() {}

    /**
     * TO be deleted? Not sure this is required
     */
    int32_t GetConfigFileLocation( char* filename, size_t& filenameLength ) override
    {
        return -1;
    }

    /**
     * @brief Retrieves the clients identity token. This token is used to identifcation/authentication when
     *   communicating with the cloud.
     */
    int32_t GetIdentity( char* token, size_t& tokenLength ) override
    {
        return -1;
    }
};
#pragma once

#include "IPmPlatformConfiguration.h"
#include "IUcLogger.h"
#include "UCIDApiDll.h"

class WindowsConfiguration : public IPmPlatformConfiguration
{
public:
    WindowsConfiguration( ICodesignVerifier& codeSignVerifier )
    : m_ucidApi(codeSignVerifier) {}
    ~WindowsConfiguration() {}

    /**
     * TO be deleted? Not sure this is required
     */
    int32_t GetConfigFileLocation( char* filename, size_t& filenameLength ) override
    {
        return -1;
    }

    /**
     * @brief Load the UCID API.
     */
    bool LoadUcidApi() override
    {
        return m_ucidApi.LoadApi();
    }

    /**
     * @brief Unload the UCID API.
     */
    void UnloadUcidApi() override
    {
        return m_ucidApi.UnloadApi();
    }

    /**
     * @brief Retrieves the clients identity id.
     */
    bool GetIdentity( std::string& id ) override
    {
        bool ret = false;

        int32_t ucidRet = m_ucidApi.GetId(id);

        if (ucidRet == 0)
        {
            ret = true;
            LOG_DEBUG("GetIdentity: %s", id.c_str());
        }
        else
        {
            LOG_ERROR("GetIdentity Failed: %d", ucidRet);
        }

        return ret;
    }

    /**
     * @brief Retrieves the clients identity token. This token is used to identifcation/authentication when
     *   communicating with the cloud.
     */
    bool GetIdentityToken( std::string& token ) override
    {
        bool ret = false;

        int32_t ucidRet = m_ucidApi.GetToken(token);

        if (ucidRet == 0)
        {
            ret = true;
            LOG_DEBUG("GetToken: %s", token.c_str());
        }
        else
        {
            LOG_ERROR("GetToken Failed: %d", ucidRet);
        }

        return ret;
    }

    /**
     * @brief Retrieves the clients identity token. This token is used to identifcation/authentication when
     *   communicating with the cloud.
     */
    bool RefreshIdentityToken() override
    {
        bool ret = false;

        int32_t ucidRet = m_ucidApi.RefreshToken();

        if (ucidRet == 0)
        {
            LOG_DEBUG("RefreshIdentityToken succeeded");
            ret = true;
        }
        else
        {
            LOG_ERROR("RefreshIdentityToken Failed: %d", ucidRet);
        }

        return ret;
    }

private:
    UCIDApiDll m_ucidApi;
};
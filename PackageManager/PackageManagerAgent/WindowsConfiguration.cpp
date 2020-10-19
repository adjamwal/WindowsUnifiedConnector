#include "pch.h"
#include "WindowsConfiguraton.h"
#include "IWinCertLoader.h"

WindowsConfiguration::WindowsConfiguration( IWinCertLoader& winCertLoader, ICodesignVerifier& codeSignVerifier ) :
    m_winCertLoader( winCertLoader ),
    m_ucidApi(codeSignVerifier)
{
    m_winCertLoader.LoadSystemCerts();
}

WindowsConfiguration::~WindowsConfiguration()
{
    m_winCertLoader.UnloadSystemCerts();
}

int32_t GetConfigFileLocation( char* filename, size_t& filenameLength )
{
    return -1;
}

bool WindowsConfiguration::LoadUcidApi()
{
    return m_ucidApi.LoadApi();
}

void WindowsConfiguration::UnloadUcidApi()
{
    return m_ucidApi.UnloadApi();
}

bool WindowsConfiguration::GetIdentity( std::string& id )
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

bool WindowsConfiguration::GetIdentityToken( std::string& token )
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

bool WindowsConfiguration::RefreshIdentityToken()
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

int32_t WindowsConfiguration::GetSslCertificates( X509*** certificates, size_t& count )
{
    return m_winCertLoader.GetSystemCerts( certificates, count );
}

void WindowsConfiguration::ReleaseSslCertificates( X509** certificates, size_t count )
{
    m_winCertLoader.FreeSystemCerts( certificates, count );
}
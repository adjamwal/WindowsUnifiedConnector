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

int32_t WindowsConfiguration::GetConfigFileLocation( char* filename, size_t& filenameLength )
{
    return -1;
}

bool WindowsConfiguration::GetIdentityToken( std::string& token )
{
    bool ret = true;
    int32_t ucidRet = 0;

    if ( m_ucidApi.LoadApi() )
    {
        //refreshh indentity token
        ucidRet = m_ucidApi.RefreshToken();

        if ( ucidRet != 0 )
        {
            LOG_ERROR( "RefreshIdentityToken Failed: %d", ucidRet );
            ret = false;
        }

        //get token
        ucidRet = m_ucidApi.GetToken( token );

        if ( ucidRet != 0 )
        {
            LOG_ERROR( "GetIdentity Failed: %d", ucidRet );
            ret = false;
        }

        m_ucidApi.UnloadApi();
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
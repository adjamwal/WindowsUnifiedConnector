#include "pch.h"
#include "WindowsConfiguraton.h"
#include "IWinCertLoader.h"

WindowsConfiguration::WindowsConfiguration( IWinCertLoader& winCertLoader ) :
    m_winCertLoader( winCertLoader )
{
    m_winCertLoader.LoadSystemCerts();
}

WindowsConfiguration::~WindowsConfiguration()
{
    m_winCertLoader.UnloadSystemCerts();
}

int32_t WindowsConfiguration::GetSslCertificates( X509*** certificates, size_t& count )
{
    return m_winCertLoader.GetSystemCerts( certificates, count );
}

void WindowsConfiguration::ReleaseSslCertificates( X509** certificates, size_t count )
{
    m_winCertLoader.FreeSystemCerts( certificates, count );
}
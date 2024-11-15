#include "pch.h"
#include "WindowsConfiguration.h"
#include "IWinCertLoader.h"
#include "IProxyDiscovery.h"
#include "..\..\GlobalVersion.h"
#include "WindowsUtilities.h"
#include "CmConstants.h"
#include <codecvt>

WindowsConfiguration::WindowsConfiguration( IWinCertLoader& winCertLoader, ICodesignVerifier& codeSignVerifier, IProxyDiscovery& proxyDiscovery ) :
    m_winCertLoader( winCertLoader ),
    m_ucidApi( codeSignVerifier ),
    m_proxyDiscovery( proxyDiscovery )
{
    m_winCertLoader.LoadSystemCerts();
    m_proxyDiscovery.RegisterForProxyNotifications( this );
}

WindowsConfiguration::~WindowsConfiguration()
{
    m_proxyDiscovery.UnregisterForProxyNotifications( this );
    m_winCertLoader.UnloadSystemCerts();

}

bool WindowsConfiguration::UpdateUCID()
{
    bool ret = true;
    int32_t ucidRet = 0;

    LOG_DEBUG( "Refreshing UCID" );
    if (m_ucidApi.LoadApi()) {
        //refresh identity token
        ucidRet = m_ucidApi.RefreshToken();

        if (ucidRet != 0) {
            LOG_ERROR("RefreshIdentityToken Failed: %d", ucidRet);
            ret = false;
        }

        //get token
        ucidRet = m_ucidApi.GetToken( m_token );

        if (ucidRet != 0) {
            LOG_ERROR("GetToken Failed: %d", ucidRet);
            ret = false;
        }

        ucidRet = m_ucidApi.GetId( m_ucid );
        if (ucidRet != 0) {
            LOG_ERROR("GetId Failed: %d", ucidRet);
            ret = false;
        }

        ucidRet = m_ucidApi.GetUrls( m_urls );
        if( ucidRet != 0 ) {
            LOG_ERROR( "GetUrls Failed: %d", ucidRet );
            ret = false;
        }

        m_ucidApi.UnloadApi();
    }

    return ret;
}

bool WindowsConfiguration::GetIdentityToken( std::string& token )
{
    std::lock_guard<std::mutex> lock( m_ucidMutex );
    bool ret = true;

    if ( m_ucid.empty() ) {
        ret = UpdateUCID();
    }

    token = m_token;

    return ret;
}

bool WindowsConfiguration::GetUcIdentity( std::string& identity )
{
    std::lock_guard<std::mutex> lock( m_ucidMutex );
    bool ret = true;

    if ( m_ucid.empty() ) {
        ret = UpdateUCID();
    }

    identity = m_ucid;

    return ret;
}

bool WindowsConfiguration::GetPmUrls( PmUrlList& urls )
{
    std::lock_guard<std::mutex> lock( m_ucidMutex );
    bool ret = true;

    if( m_urls.catalogUrl.empty() || m_urls.checkinUrl.empty() || m_urls.eventUrl.empty() ) {
        ret = UpdateUCID();
    }

    urls = m_urls;

    return ret;
}

bool WindowsConfiguration::RefreshIdentity()
{
    std::lock_guard<std::mutex> lock( m_ucidMutex );
    return UpdateUCID();
}

int32_t WindowsConfiguration::ReloadSslCertificates()
{
    std::lock_guard<std::mutex> lock( m_certMutex );

    int32_t rtn = m_winCertLoader.UnloadSystemCerts();

    if( rtn != 0 ) {
        LOG_ERROR( "Failed to UnloadSystemCerts" );
    }
    else {
        if( ( rtn = m_winCertLoader.LoadSystemCerts() ) != 0 ) {
            LOG_ERROR( "Failed to LoadSystemCerts" );
        }
    }

    return rtn;
}

int32_t WindowsConfiguration::GetSslCertificates( X509*** certificates, size_t& count )
{
    std::lock_guard<std::mutex> lock( m_certMutex );

    return m_winCertLoader.GetSystemCerts( certificates, count );
}

void WindowsConfiguration::ReleaseSslCertificates( X509** certificates, size_t count )
{
    std::lock_guard<std::mutex> lock( m_certMutex );

    m_winCertLoader.FreeSystemCerts( certificates, count );
}

std::string WindowsConfiguration::GetHttpUserAgent()
{
    std::string agent;

    agent = "PackageManager/" + GetPmVersion();
    return agent;
}

std::string WindowsConfiguration::GetInstallDirectory()
{
    std::wstring dir;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    if ( !WindowsUtilities::ReadRegistryStringW( HKEY_LOCAL_MACHINE, WREG_CM, L"Path", dir ) )
    {
        WLOG_ERROR( L"Failed to read Cloud Management install path from registry" );
    }

    return converter.to_bytes( dir );
}

std::string WindowsConfiguration::GetDataDirectory()
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    return converter.to_bytes( WindowsUtilities::GetLogDir() );
}

std::string WindowsConfiguration::GetPmVersion()
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    return converter.to_bytes( STRFORMATPRODVER );
}

bool WindowsConfiguration::UpdateCertStoreForUrl( const std::string& url )
{
    bool rtn = false;
    std::string urlRoot = ExtractUrlRoot( url );

    if( !urlRoot.empty() ) {
        m_certChaninUrlMap[ urlRoot ] = 0;
    }

    for( auto kv : m_certChaninUrlMap ) {
        rtn = WindowsUtilities::WinHttpGet( kv.first );
    }

    return rtn;
}

std::list<PmProxy> WindowsConfiguration::StartProxyDiscovery(const std::string& testUrl, const std::string& pacUrl)
{
    std::list<PmProxy> pmProxyList;
    std::list<ProxyInfoModel> proxyList;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    m_proxyDiscovery.ProxyDiscoverySync(converter.from_bytes(testUrl).c_str(), converter.from_bytes(pacUrl).c_str(), proxyList);

    for (auto& proxy : proxyList) {
        pmProxyList.push_back({ converter.to_bytes(proxy.GetProxyServer()), proxy.GetProxyPort(), converter.to_bytes(proxy.GetProxyType()) });
    }

    return pmProxyList;
}

bool WindowsConfiguration::StartProxyDiscoveryAsync(const std::string& testUrl, const std::string& pacUrl, AsyncProxyDiscoveryCb cb, void* context)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    if (cb) {
        std::unique_lock lock(m_proxyMutex);
        m_proxy_cb = cb;
        m_proxy_context = context;

        m_proxyDiscovery.StartProxyDiscoveryAsync(converter.from_bytes(testUrl).c_str(), converter.from_bytes(pacUrl).c_str());
        return true;
    }

    return false;
}

void WindowsConfiguration::ProxiesDiscovered(const std::list<ProxyInfoModel>& proxySettings)
{
    std::unique_lock lock(m_proxyMutex);
    if (m_proxy_cb) {
        std::list<PmProxy> pmProxyList;
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

        for (auto& proxy : proxySettings) {
            pmProxyList.push_back({ converter.to_bytes(proxy.GetProxyServer()), proxy.GetProxyPort(), converter.to_bytes(proxy.GetProxyType()) });
        }

        m_proxy_cb(m_proxy_context, pmProxyList);

        m_proxy_cb = nullptr;
        m_proxy_context = nullptr;
    }
}

std::string WindowsConfiguration::ExtractUrlRoot( const std::string& url )
{
    size_t begin = url.find( "://" );
    if( begin == std::string::npos ) {
        return "";
    }

    begin += 3;
    size_t end = url.find( '/', begin );

    if( end == std::string::npos ) {
        return url.substr( begin );
    }
    
    return url.substr( begin, end - begin );
}

/**
 * @brief Retrieve the PM platform name, accepted by backend
 */
std::string WindowsConfiguration::GetPmPlatform()
{
    return "windows";
}

/**
 * @brief Retrieve the PM architecture name, accepted by backend
 */
std::string WindowsConfiguration::GetPmArchitecture()
{
    return WindowsUtilities::GetSystemArchitecture();
}
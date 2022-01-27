#include "pch.h"
#include "ProxyContainer.h"
#include "IProxyDiscovery.h"
#include "IProxyConsumer.h"
#include "WinHttpWrapper.h"
#include <Windows.h>
#include <winhttp.h>
#include <memory>
#include "fff_with_cc.h"

DEFINE_FFF_GLOBALS

#pragma warning( push )
#pragma warning( disable : 4217; disable : 4273 )

#if defined BOOLAPI
#undef BOOLAPI
#endif
#define BOOLAPI BOOL

extern "C" {
    FAKE_VALUE_FUNC( BOOL, WINAPI, WinHttpGetDefaultProxyConfiguration, IN OUT WINHTTP_PROXY_INFO* );
    FAKE_VALUE_FUNC( BOOL, WINAPI, WinHttpGetIEProxyConfigForCurrentUser, IN OUT WINHTTP_CURRENT_USER_IE_PROXY_CONFIG* );
    FAKE_VALUE_FUNC( HINTERNET, WINAPI, WinHttpOpen, _In_opt_z_ LPCWSTR, _In_ DWORD, _In_opt_z_ LPCWSTR, _In_opt_z_ LPCWSTR, _In_ DWORD );
    FAKE_VALUE_FUNC( BOOLAPI, WINAPI, WinHttpGetProxyForUrl, IN  HINTERNET, IN  LPCWSTR, IN  WINHTTP_AUTOPROXY_OPTIONS*, OUT WINHTTP_PROXY_INFO* );
    FAKE_VALUE_FUNC( BOOLAPI, WINAPI, WinHttpCloseHandle, IN HINTERNET );
}

#pragma warning( pop )

WINHTTP_PROXY_INFO g_testDefaultProxyInfo = { 0 };
BOOL WINAPI WinHttpGetDefaultProxyConfiguration_custom_fake( IN OUT WINHTTP_PROXY_INFO* pProxyInfo )
{
    *pProxyInfo = { 0 };
    pProxyInfo->dwAccessType = g_testDefaultProxyInfo.dwAccessType;
    if( g_testDefaultProxyInfo.lpszProxy )
    {
        pProxyInfo->lpszProxy = ( LPWSTR )GlobalAlloc( GMEM_FIXED, ( wcslen( g_testDefaultProxyInfo.lpszProxy ) + 1 ) * sizeof( WCHAR ) );
        wmemcpy( pProxyInfo->lpszProxy, g_testDefaultProxyInfo.lpszProxy, wcslen( g_testDefaultProxyInfo.lpszProxy ) + 1 );
    }
    if( g_testDefaultProxyInfo.lpszProxyBypass )
    {
        pProxyInfo->lpszProxyBypass = ( LPWSTR )GlobalAlloc( GMEM_FIXED, ( wcslen( g_testDefaultProxyInfo.lpszProxyBypass ) + 1 ) * sizeof( WCHAR ) );
        wmemcpy( pProxyInfo->lpszProxyBypass, g_testDefaultProxyInfo.lpszProxyBypass, wcslen( g_testDefaultProxyInfo.lpszProxyBypass ) + 1 );
    }
    return TRUE;
}

WINHTTP_CURRENT_USER_IE_PROXY_CONFIG g_testUserIEProxyInfo = { 0 };
BOOL WINAPI WinHttpGetIEProxyConfigForCurrentUser_custom_fake( IN OUT WINHTTP_CURRENT_USER_IE_PROXY_CONFIG* pIeProxyInfo )
{
    *pIeProxyInfo = { 0 };
    pIeProxyInfo->fAutoDetect = g_testUserIEProxyInfo.fAutoDetect;
    if( g_testUserIEProxyInfo.lpszAutoConfigUrl )
    {
        pIeProxyInfo->lpszAutoConfigUrl = ( LPWSTR )GlobalAlloc( GMEM_FIXED, ( wcslen( g_testUserIEProxyInfo.lpszAutoConfigUrl ) + 1 ) * sizeof( WCHAR ) );
        wmemcpy( pIeProxyInfo->lpszAutoConfigUrl, g_testUserIEProxyInfo.lpszAutoConfigUrl, wcslen( g_testUserIEProxyInfo.lpszAutoConfigUrl ) + 1 );
    }
    if( g_testUserIEProxyInfo.lpszProxy )
    {
        pIeProxyInfo->lpszProxy = ( LPWSTR )GlobalAlloc( GMEM_FIXED, ( wcslen( g_testUserIEProxyInfo.lpszProxy ) + 1 ) * sizeof( WCHAR ) );
        wmemcpy( pIeProxyInfo->lpszProxy, g_testUserIEProxyInfo.lpszProxy, wcslen( g_testUserIEProxyInfo.lpszProxy ) + 1 );
    }
    if( g_testUserIEProxyInfo.lpszProxyBypass )
    {
        pIeProxyInfo->lpszProxyBypass = ( LPWSTR )GlobalAlloc( GMEM_FIXED, ( wcslen( g_testUserIEProxyInfo.lpszProxyBypass ) + 1 ) * sizeof( WCHAR ) );
        wmemcpy( pIeProxyInfo->lpszProxyBypass, g_testUserIEProxyInfo.lpszProxyBypass, wcslen( g_testUserIEProxyInfo.lpszProxyBypass ) + 1 );
    }
    return TRUE;
}

WINHTTP_PROXY_INFO g_testProxyInfoForUrl = { 0 };
BOOL WINAPI WinHttpGetProxyForUrl_custom_fake( HINTERNET hSession, LPCWSTR lpcwszUrl, WINHTTP_AUTOPROXY_OPTIONS* pAutoProxyOptions, WINHTTP_PROXY_INFO* pProxyInfo )
{
    *pProxyInfo = { 0 };
    pProxyInfo->dwAccessType = g_testProxyInfoForUrl.dwAccessType;
    if( g_testProxyInfoForUrl.lpszProxy )
    {
        pProxyInfo->lpszProxy = ( LPWSTR )GlobalAlloc( GMEM_FIXED, ( wcslen( g_testProxyInfoForUrl.lpszProxy ) + 1 ) * sizeof( WCHAR ) );
        wmemcpy( pProxyInfo->lpszProxy, g_testProxyInfoForUrl.lpszProxy, wcslen( g_testProxyInfoForUrl.lpszProxy ) + 1 );
    }
    if( g_testProxyInfoForUrl.lpszProxyBypass )
    {
        pProxyInfo->lpszProxyBypass = ( LPWSTR )GlobalAlloc( GMEM_FIXED, ( wcslen( g_testProxyInfoForUrl.lpszProxyBypass ) + 1 ) * sizeof( WCHAR ) );
        wmemcpy( pProxyInfo->lpszProxyBypass, g_testProxyInfoForUrl.lpszProxyBypass, wcslen( g_testProxyInfoForUrl.lpszProxyBypass ) + 1 );
    }
    return TRUE;
}

class ComponentTestProxy : public ::testing::Test
{
protected:
    void ResetFakes()
    {
        if( g_testDefaultProxyInfo.lpszProxy ) GlobalFree( g_testDefaultProxyInfo.lpszProxy );
        if( g_testDefaultProxyInfo.lpszProxyBypass ) GlobalFree( g_testDefaultProxyInfo.lpszProxyBypass );
        g_testDefaultProxyInfo = { 0 };

        if( g_testUserIEProxyInfo.lpszAutoConfigUrl ) GlobalFree( g_testUserIEProxyInfo.lpszAutoConfigUrl );
        if( g_testUserIEProxyInfo.lpszProxy ) GlobalFree( g_testUserIEProxyInfo.lpszProxy );
        if( g_testUserIEProxyInfo.lpszProxyBypass ) GlobalFree( g_testUserIEProxyInfo.lpszProxyBypass );
        g_testUserIEProxyInfo = { 0 };

        if( g_testProxyInfoForUrl.lpszProxy ) GlobalFree( g_testProxyInfoForUrl.lpszProxy );
        if( g_testProxyInfoForUrl.lpszProxyBypass ) GlobalFree( g_testProxyInfoForUrl.lpszProxyBypass );
        g_testProxyInfoForUrl = { 0 };

        RESET_FAKE( WinHttpGetDefaultProxyConfiguration );
        RESET_FAKE( WinHttpGetIEProxyConfigForCurrentUser );
        RESET_FAKE( WinHttpOpen );
        RESET_FAKE( WinHttpGetProxyForUrl );
        RESET_FAKE( WinHttpCloseHandle );
        FFF_RESET_HISTORY();
    }

    void SetUp()
    {
        ResetFakes();
        g_testProxyInfoForUrl = { 0 };
        m_container.reset( new ProxyContainer() );
    }

    void TearDown()
    {
        m_container.reset();
    }

    std::unique_ptr<ProxyContainer> m_container;
};

TEST_F( ComponentTestProxy, ExpectedProxyDiscoverySync )
{
    std::wstring testUrl( L"testUrl" );
    std::wstring pacURL( L"pacURL" );
    std::list<ProxyInfoModel> proxyList {};

    g_testDefaultProxyInfo = { WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, L"systemProxy", NULL };
    g_testUserIEProxyInfo = { false, NULL, L"ieProxy", NULL };
    g_testProxyInfoForUrl = { WINHTTP_ACCESS_TYPE_NAMED_PROXY, L"proxy1;proxy2", NULL };
    WinHttpGetDefaultProxyConfiguration_fake.custom_fake = WinHttpGetDefaultProxyConfiguration_custom_fake;
    WinHttpGetIEProxyConfigForCurrentUser_fake.custom_fake = WinHttpGetIEProxyConfigForCurrentUser_custom_fake;
    WinHttpGetProxyForUrl_fake.custom_fake = WinHttpGetProxyForUrl_custom_fake;
    WinHttpOpen_fake.return_val = ( HINTERNET )1;
    WinHttpCloseHandle_fake.return_val = TRUE;

    IProxyDiscovery& m_patient = m_container->GetProxyDiscovery();
    m_patient.ProxyDiscoverySync( testUrl.c_str(), pacURL.c_str(), proxyList );

    EXPECT_EQ( 1, WinHttpGetDefaultProxyConfiguration_fake.call_count );
    EXPECT_EQ( 1, WinHttpGetIEProxyConfigForCurrentUser_fake.call_count );
    ASSERT_EQ( 8, proxyList.size() );
    std::wstring expect[] = { L"systemProxy", L"ieProxy", L"proxy1", L"proxy2", L"proxy1", L"proxy2", L"proxy1", L"proxy2" };
    int i = 0;
    for( auto it = proxyList.begin(); it != proxyList.end(); ++it )
    {
        EXPECT_TRUE( expect[ i++ ] == it->GetProxyServer() );
    }
}



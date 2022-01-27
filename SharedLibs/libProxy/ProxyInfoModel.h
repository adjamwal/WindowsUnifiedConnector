#pragma once

#include <Windows.h>
#include <string>
#include <list>

//Duplicated from curl.h
#define PROXY_AUTH_NONE         ((unsigned long)0)
#define PROXY_AUTH_BASIC        (((unsigned long)1)<<0)
#define PROXY_AUTH_NTLM         (((unsigned long)1)<<3)

class ProxyInfoModel
{
public:
    ProxyInfoModel();
    virtual ~ProxyInfoModel();

    BOOL IsValid();

    void SetProxyPort( USHORT port );
    USHORT GetProxyPort() const;

    void SetProxyAuthType( DWORD proxyAuthType );
    DWORD GetProxyAuthType() const;

    void SetProxyTunnel( DWORD tunnel );
    DWORD GetProxyTunnel() const;

    void SetProxyType( const std::wstring& proxyType );
    const std::wstring& GetProxyType() const;

    void SetProxyAccessType( const std::wstring& proxyAccessType );
    const std::wstring& GetProxyAccessType() const;

    void SetProxyServer( const std::wstring& proxyServer );
    const std::wstring& GetProxyServer() const;

    void SetProxyUser( const std::wstring& proxyUserName );
    const std::wstring& GetProxyUser() const;

    void SetProxyPassword( const std::wstring& proxyPassword );
    const std::wstring& GetProxyPassword() const;

    void SetProxyDiscoveryMode( DWORD proxyDiscoveryMode );
    DWORD GetProxyDiscoveryMode() const;
    void Clear();

    void operator=( const ProxyInfoModel& other );
    bool operator==( const ProxyInfoModel& rhs ) const;
    bool operator!=( const ProxyInfoModel& rhs ) const;

private:
    USHORT m_proxyPort;
    DWORD m_proxyAuthType;
    DWORD m_tunnel;
    DWORD m_proxyDiscoveryMode;
    std::wstring m_proxyType;
    std::wstring m_proxyServer;
    std::wstring m_proxyUname;
    std::wstring m_proxyPassword;
    std::wstring m_accessType;
};

typedef std::list<ProxyInfoModel> PROXY_INFO_LIST;

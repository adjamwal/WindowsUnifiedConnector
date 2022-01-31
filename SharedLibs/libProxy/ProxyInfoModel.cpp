#include "ProxyInfoModel.h"

ProxyInfoModel::ProxyInfoModel()
    : m_proxyPort( 0 )
    , m_proxyAuthType( 0 )
    , m_tunnel( 0 )
    , m_proxyDiscoveryMode( -1 )
    , m_proxyType( L"" )
    , m_proxyServer( L"" )
    , m_proxyUname( L"" )
    , m_proxyPassword( L"" )
    , m_accessType( L"" )
{
}

ProxyInfoModel::ProxyInfoModel(
    USHORT proxyPort,
    DWORD proxyAuthType,
    DWORD tunnel,
    DWORD proxyDiscoveryMode,
    std::wstring proxyType,
    std::wstring proxyServer,
    std::wstring proxyUname,
    std::wstring proxyPassword,
    std::wstring accessType )
    : m_proxyPort( proxyPort )
    , m_proxyAuthType( proxyAuthType )
    , m_tunnel( tunnel )
    , m_proxyDiscoveryMode( proxyDiscoveryMode )
    , m_proxyType( proxyType )
    , m_proxyServer( proxyServer )
    , m_proxyUname( proxyUname )
    , m_proxyPassword( proxyPassword )
    , m_accessType( accessType )
{
}

ProxyInfoModel::~ProxyInfoModel()
{
    m_proxyPassword.clear();
}

BOOL ProxyInfoModel::IsValid()
{
    return ( m_proxyType.empty() ? FALSE : TRUE );
}

void ProxyInfoModel::SetProxyPort( USHORT port )
{
    m_proxyPort = port;
}

USHORT ProxyInfoModel::GetProxyPort() const
{
    return m_proxyPort;
}

void ProxyInfoModel::SetProxyAuthType( DWORD proxyAuthType )
{
    m_proxyAuthType = proxyAuthType;
}

DWORD ProxyInfoModel::GetProxyAuthType() const
{
    return m_proxyAuthType;
}

void ProxyInfoModel::SetProxyTunnel( DWORD tunnel )
{
    m_tunnel = tunnel;
}

DWORD ProxyInfoModel::GetProxyTunnel() const
{
    return m_tunnel;
}

void ProxyInfoModel::SetProxyType( const std::wstring &proxyType )
{
    m_proxyType = proxyType;
}

const std::wstring & ProxyInfoModel::GetProxyType() const
{
    return m_proxyType;
}

void ProxyInfoModel::SetProxyAccessType( const std::wstring &proxyAccessType )
{
    m_accessType = proxyAccessType;
}

const std::wstring & ProxyInfoModel::GetProxyAccessType() const
{
    return m_accessType;
}

void ProxyInfoModel::SetProxyServer( const std::wstring &proxyServer )
{
    m_proxyServer = proxyServer;
}

const std::wstring& ProxyInfoModel::GetProxyServer() const
{
    return m_proxyServer;
}

void ProxyInfoModel::SetProxyUser( const std::wstring &proxyUserName )
{
    m_proxyUname = proxyUserName;
}

const std::wstring & ProxyInfoModel::GetProxyUser() const
{
    return m_proxyUname;
}

void ProxyInfoModel::SetProxyPassword( const std::wstring &proxyPassword )
{
    m_proxyPassword = proxyPassword;
}

const std::wstring & ProxyInfoModel::GetProxyPassword() const
{
    return m_proxyPassword;
}

void ProxyInfoModel::SetProxyDiscoveryMode( DWORD proxyDiscoveryMode )
{
    m_proxyDiscoveryMode = proxyDiscoveryMode;
}

DWORD ProxyInfoModel::GetProxyDiscoveryMode() const
{
    return m_proxyDiscoveryMode;
}

void ProxyInfoModel::operator=( const ProxyInfoModel& other )
{
    m_proxyPort = other.m_proxyPort;
    m_proxyAuthType = other.m_proxyAuthType;
    m_tunnel = other.m_tunnel;
    m_proxyDiscoveryMode = other.m_proxyDiscoveryMode;
    m_proxyType = other.m_proxyType;
    m_proxyServer = other.m_proxyServer;
    m_proxyUname = other.m_proxyUname;
    m_proxyPassword = other.m_proxyPassword;
    m_accessType = other.m_accessType;
}

bool ProxyInfoModel::operator!=( const ProxyInfoModel& rhs ) const
{
    return !( *this == rhs );
}

bool ProxyInfoModel::operator==( const ProxyInfoModel& rhs ) const
{
    return
        ( m_proxyPort == rhs.m_proxyPort ) &&
        ( m_proxyAuthType == rhs.m_proxyAuthType ) &&
        ( m_tunnel == rhs.m_tunnel ) &&
        ( m_proxyDiscoveryMode == rhs.m_proxyDiscoveryMode ) &&
        ( m_proxyType == rhs.m_proxyType ) &&
        ( m_proxyServer == rhs.m_proxyServer ) &&
        ( m_proxyUname == rhs.m_proxyUname ) &&
        ( m_proxyPassword == rhs.m_proxyPassword ) &&
        ( m_accessType == rhs.m_accessType );
}

void ProxyInfoModel::Clear()
{
    m_proxyPort = 0;
    m_proxyAuthType = 0;
    m_tunnel = 0;
    m_proxyDiscoveryMode = -1;
    m_proxyType.clear();
    m_proxyServer.clear();
    m_proxyUname.clear();
    m_proxyPassword.clear();
    m_accessType.clear();
}

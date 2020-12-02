#pragma once
#include "MocksCommon.h"

#include "IPmPlatformConfiguration.h"

class MockPmPlatformConfiguration : public IPmPlatformConfiguration
{
public:
    MockPmPlatformConfiguration();
    ~MockPmPlatformConfiguration();

    MOCK_METHOD1( GetIdentityToken, bool( std::string& ) );
    void MakeGetIdentityTokenReturn( bool value );
    void ExpectGetIdentityTokenIsNotCalled();

    MOCK_METHOD1( GetUcIdentity, bool( std::string& ) );
    void MakeGetUcIdentityReturn( bool value );
    void ExpectGetUcIdentityIsNotCalled();

    MOCK_METHOD0( RefreshIdentity, bool() );
    void MakeRefreshIdentityReturn( bool value );
    void ExpectRefreshIdentityIsNotCalled();

    MOCK_METHOD2( GetSslCertificates, int32_t( X509***, size_t & ) );
    void MakeGetSslCertificatesReturn( int32_t value );
    void ExpectGetSslCertificatesIsNotCalled();

    MOCK_METHOD2( ReleaseSslCertificates, void( X509**, size_t ) );
    void ExpectReleaseSslCertificatesIsNotCalled();

    MOCK_METHOD0( GetHttpUserAgent, std::string() );
    void MakeGetHttpUserAgentReturn( std::string value );
    void ExpectGetHttpUserAgentIsNotCalled();

private:
    std::string m_defaultUserAgent;
};

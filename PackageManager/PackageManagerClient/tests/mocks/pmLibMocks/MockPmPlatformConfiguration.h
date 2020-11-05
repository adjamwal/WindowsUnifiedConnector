#pragma once
#include "MocksCommon.h"

#include "IPmPlatformConfiguration.h"

class MockPmPlatformConfiguration : public IPmPlatformConfiguration
{
public:
    MockPmPlatformConfiguration();
    ~MockPmPlatformConfiguration();

    MOCK_METHOD2( GetConfigFileLocation, int32_t( char*, size_t& ) );
    void MakeGetConfigFileLocationReturn( int32_t value );
    void ExpectGetConfigFileLocationIsNotCalled();

    MOCK_METHOD1( GetIdentityToken, bool( std::string& ) );
    void MakeGetIdentityTokenReturn( bool value );
    void ExpectGetIdentityTokenIsNotCalled();

    MOCK_METHOD2( GetSslCertificates, int32_t( X509***, size_t & ) );
    void MakeGetSslCertificatesReturn( int32_t value );
    void ExpectGetSslCertificatesIsNotCalled();

    MOCK_METHOD2( ReleaseSslCertificates, void( X509**, size_t ) );
    void ExpectReleaseSslCertificatesIsNotCalled();

};

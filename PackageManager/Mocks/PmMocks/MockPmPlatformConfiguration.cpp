#include "MockPmPlatformConfiguration.h"

MockPmPlatformConfiguration::MockPmPlatformConfiguration()
{
    MakeGetIdentityTokenReturn( false );
    MakeGetSslCertificatesReturn( int32_t() );
    MakeGetHttpUserAgentReturn( m_defaultUserAgent );
}

MockPmPlatformConfiguration::~MockPmPlatformConfiguration()
{
}

void MockPmPlatformConfiguration::MakeGetIdentityTokenReturn( bool value )
{
    ON_CALL( *this, GetIdentityToken( _ ) ).WillByDefault( Return( value ) );
}

void MockPmPlatformConfiguration::ExpectGetIdentityTokenIsNotCalled()
{
    EXPECT_CALL( *this, GetIdentityToken( _ ) ).Times( 0 );
}

void MockPmPlatformConfiguration::MakeGetSslCertificatesReturn( int32_t value )
{
    ON_CALL( *this, GetSslCertificates( _, _ ) ).WillByDefault( Return( value ) );
}

void MockPmPlatformConfiguration::ExpectGetSslCertificatesIsNotCalled()
{
    EXPECT_CALL( *this, GetSslCertificates( _, _ ) ).Times( 0 );
}

void MockPmPlatformConfiguration::ExpectReleaseSslCertificatesIsNotCalled()
{
    EXPECT_CALL( *this, ReleaseSslCertificates( _, _ ) ).Times( 0 );
}

void MockPmPlatformConfiguration::MakeGetHttpUserAgentReturn( std::string value )
{
    ON_CALL( *this, GetHttpUserAgent() ).WillByDefault( Return( value ) );
}

void MockPmPlatformConfiguration::ExpectGetHttpUserAgentIsNotCalled()
{
    EXPECT_CALL( *this, GetHttpUserAgent() ).Times( 0 );
}

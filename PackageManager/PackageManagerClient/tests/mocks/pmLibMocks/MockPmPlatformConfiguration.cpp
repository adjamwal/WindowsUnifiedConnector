#include "MockPmPlatformConfiguration.h"

MockPmPlatformConfiguration::MockPmPlatformConfiguration()
{
    MakeGetConfigFileLocationReturn( int32_t() );
    MakeGetIdentityTokenReturn( false );
    MakeGetSslCertificatesReturn( int32_t() );
}

MockPmPlatformConfiguration::~MockPmPlatformConfiguration()
{
}

void MockPmPlatformConfiguration::MakeGetConfigFileLocationReturn( int32_t value )
{
    ON_CALL( *this, GetConfigFileLocation( _, _ ) ).WillByDefault( Return( value ) );
}

void MockPmPlatformConfiguration::ExpectGetConfigFileLocationIsNotCalled()
{
    EXPECT_CALL( *this, GetConfigFileLocation( _, _ ) ).Times( 0 );
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


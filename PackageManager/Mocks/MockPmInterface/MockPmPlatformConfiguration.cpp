#include "MockPmPlatformConfiguration.h"

MockPmPlatformConfiguration::MockPmPlatformConfiguration()
{
    MakeGetIdentityTokenReturn( false );
    MakeGetSslCertificatesReturn( int32_t() );
    MakeGetHttpUserAgentReturn( m_defaultUserAgent );
    MakeGetPmVersionReturn( "" );
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

void MockPmPlatformConfiguration::MakeGetUcIdentityReturn( bool value )
{
    ON_CALL( *this, GetUcIdentity( _ ) ).WillByDefault( Return( value ) );
}

void MockPmPlatformConfiguration::ExpectGetUcIdentityIsNotCalled()
{
    EXPECT_CALL( *this, GetUcIdentity( _ ) ).Times( 0 );
}

void MockPmPlatformConfiguration::MakeRefreshIdentityReturn( bool value )
{
    ON_CALL( *this, RefreshIdentity() ).WillByDefault( Return( value ) );
}

void MockPmPlatformConfiguration::ExpectRefreshIdentityIsNotCalled()
{
    EXPECT_CALL( *this, RefreshIdentity() ).Times( 0 );
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

void MockPmPlatformConfiguration::MakeGetInstallDirectoryReturn( std::string value )
{
    ON_CALL( *this, GetInstallDirectory() ).WillByDefault( Return( value ) );
}

void MockPmPlatformConfiguration::ExpectGetInstallDirectoryIsNotCalled()
{
    EXPECT_CALL( *this, GetInstallDirectory() ).Times( 0 );
}

void MockPmPlatformConfiguration::MakeGetDataDirectoryReturn( std::string value )
{
    ON_CALL( *this, GetDataDirectory() ).WillByDefault( Return( value ) );
}

void MockPmPlatformConfiguration::ExpectGetDataDirectoryIsNotCalled()
{
    EXPECT_CALL( *this, GetDataDirectory() ).Times( 0 );
}

void MockPmPlatformConfiguration::MakeGetPmVersionReturn( std::string value )
{
    ON_CALL( *this, GetPmVersion() ).WillByDefault( Return( value ) );
}

void MockPmPlatformConfiguration::ExpectGetPmVersionIsNotCalled()
{
    EXPECT_CALL( *this, GetPmVersion() ).Times( 0 );
}

void MockPmPlatformConfiguration::MakeGetPmUrlsReturn( bool value )
{
    ON_CALL( *this, GetPmUrls( _ ) ).WillByDefault( Return( value ) );
}

void MockPmPlatformConfiguration::ExpectGetPmUrlsIsNotCalled()
{
    EXPECT_CALL( *this, GetPmUrls( _ ) ).Times( 0 );
}
#include "MockPmPlatformComponentManager.h"

MockPmPlatformComponentManager::MockPmPlatformComponentManager()
{
    MakeGetInstalledPackagesReturn( int32_t() );
    MakeInstallComponentReturn( int32_t() );
    MakeUpdateComponentReturn( int32_t() );
    MakeUninstallComponentReturn( int32_t() );
    MakeDeployConfigurationReturn( int32_t() );
    MakeResolvePathReturn( "" );
}

MockPmPlatformComponentManager::~MockPmPlatformComponentManager()
{
}

void MockPmPlatformComponentManager::MakeGetInstalledPackagesReturn( int32_t value )
{
    ON_CALL( *this, GetInstalledPackages( _ ) ).WillByDefault( Return( value ) );
}

void MockPmPlatformComponentManager::ExpectGetInstalledPackagesIsNotCalled()
{
    EXPECT_CALL( *this, GetInstalledPackages( _ ) ).Times( 0 );
}

void MockPmPlatformComponentManager::MakeInstallComponentReturn( int32_t value )
{
    ON_CALL( *this, InstallComponent( _ ) ).WillByDefault( Return( value ) );
}

void MockPmPlatformComponentManager::ExpectInstallComponentIsNotCalled()
{
    EXPECT_CALL( *this, InstallComponent( _ ) ).Times( 0 );
}

void MockPmPlatformComponentManager::MakeUpdateComponentReturn( int32_t value )
{
    ON_CALL( *this, UpdateComponent( _, _ ) ).WillByDefault( Return( value ) );
}

void MockPmPlatformComponentManager::ExpectUpdateComponentIsNotCalled()
{
    EXPECT_CALL( *this, UpdateComponent( _, _ ) ).Times( 0 );
}

void MockPmPlatformComponentManager::MakeUninstallComponentReturn( int32_t value )
{
    ON_CALL( *this, UninstallComponent( _ ) ).WillByDefault( Return( value ) );
}

void MockPmPlatformComponentManager::ExpectUninstallComponentIsNotCalled()
{
    EXPECT_CALL( *this, UninstallComponent( _ ) ).Times( 0 );
}

void MockPmPlatformComponentManager::MakeDeployConfigurationReturn( int32_t value )
{
    ON_CALL( *this, DeployConfiguration( _ ) ).WillByDefault( Return( value ) );
}

void MockPmPlatformComponentManager::ExpectDeployConfigurationIsNotCalled()
{
    EXPECT_CALL( *this, DeployConfiguration( _ ) ).Times( 0 );
}

void MockPmPlatformComponentManager::MakeResolvePathReturn( std::string value )
{
    ON_CALL( *this, ResolvePath( _, _ ) ).WillByDefault( Return( value ) );
}
void MockPmPlatformComponentManager::ExpectResolvePathIsNotCalled()
{
    EXPECT_CALL( *this, ResolvePath( _, _ ) ).Times( 0 );
}
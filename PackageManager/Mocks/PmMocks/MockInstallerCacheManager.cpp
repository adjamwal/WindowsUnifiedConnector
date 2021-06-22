#include "MockInstallerCacheManager.h"

MockInstallerCacheManager::MockInstallerCacheManager()
{
    MakeDownloadOrUpdateInstallerReturn( "" );
}

MockInstallerCacheManager::~MockInstallerCacheManager()
{

}

void MockInstallerCacheManager::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockInstallerCacheManager::MakeDownloadOrUpdateInstallerReturn( const std::filesystem::path& value )
{
    ON_CALL( *this, DownloadOrUpdateInstaller( _ ) ).WillByDefault( Return( value ) );
}

void MockInstallerCacheManager::MakeDownloadOrUpdateInstallerThrow( std::string message, int code )
{
    ON_CALL( *this, DownloadOrUpdateInstaller( _ ) ).WillByDefault(
        Throw( PackageException( message, code ) ) );
}

void MockInstallerCacheManager::ExpectDownloadOrUpdateInstallerIsNotCalled()
{
    EXPECT_CALL( *this, DownloadOrUpdateInstaller( _ ) ).Times( 0 );
}

void MockInstallerCacheManager::ExpectDeleteInstallerIsNotCalled()
{
    EXPECT_CALL( *this, DeleteInstaller( _ ) ).Times( 0 );
}

void MockInstallerCacheManager::ExpectPruneInstallersIsNotCalled()
{
    EXPECT_CALL( *this, PruneInstallers( _ ) ).Times( 0 );
}
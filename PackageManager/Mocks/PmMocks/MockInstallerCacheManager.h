#pragma once
#include "MocksCommon.h"
#include "PmTypes.h"
#include "PackageException.h"

#include "IInstallerCacheManager.h"

class MockInstallerCacheManager : public IInstallerCacheManager
{
public:
    MockInstallerCacheManager();
    ~MockInstallerCacheManager();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies*) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD1( DownloadOrUpdateInstaller, std::filesystem::path( const PmComponent& ) );
    void MakeDownloadOrUpdateInstallerReturn( const std::filesystem::path& value );
    void MakeDownloadOrUpdateInstallerThrow( std::string message, int code );
    void ExpectDownloadOrUpdateInstallerIsNotCalled();

    MOCK_METHOD1( DeleteInstaller, void( const std::filesystem::path& ) );
    void ExpectDeleteInstallerIsNotCalled();

    MOCK_METHOD1( PruneInstallers, void( uint32_t ) );
    void ExpectPruneInstallersIsNotCalled();
};

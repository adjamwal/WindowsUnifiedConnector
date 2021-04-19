#pragma once
#include "MocksCommon.h"
#include "PmTypes.h"

#include "IInstallerCacheManager.h"

class MockInstallerCacheManager : public IInstallerCacheManager
{
public:
    MockInstallerCacheManager();
    ~MockInstallerCacheManager();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies*) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD1( DownloadOrUpdateInstaller, std::string( const PmComponent& ) );
    void MakeDownloadOrUpdateInstallerReturn( const std::string& value );
    void ExpectDownloadOrUpdateInstallerIsNotCalled();

    MOCK_METHOD1( DeleteInstaller, void( const std::string& ) );
    void ExpectDeleteInstallerIsNotCalled();

    MOCK_METHOD1( PruneInstallers, void( uint32_t ) );
    void ExpectPruneInstallersIsNotCalled();
};

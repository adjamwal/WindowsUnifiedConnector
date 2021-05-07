#pragma once
#include "MocksCommon.h"

#include "IPmPlatformComponentManager.h"
#include "PmTypes.h"

class MockPmPlatformComponentManager : public IPmPlatformComponentManager
{
public:
    MockPmPlatformComponentManager();
    ~MockPmPlatformComponentManager();

    MOCK_METHOD2( GetInstalledPackages, int32_t( const std::vector<PmProductDiscoveryRules>&, PackageInventory& ) );
    void MakeGetInstalledPackagesReturn( int32_t value );
    void ExpectGetInstalledPackagesIsNotCalled();

    MOCK_METHOD1( GetCachedInventory, int32_t( PackageInventory& ) );

    MOCK_METHOD1( InstallComponent, int32_t( const PmComponent& ) );
    void MakeInstallComponentReturn( int32_t value );
    void ExpectInstallComponentIsNotCalled();

    MOCK_METHOD2( UpdateComponent, int32_t( const PmComponent&, std::string& ) );
    void MakeUpdateComponentReturn( int32_t value );
    void ExpectUpdateComponentIsNotCalled();

    MOCK_METHOD1( UninstallComponent, int32_t( const PmComponent& ) );
    void MakeUninstallComponentReturn( int32_t value );
    void ExpectUninstallComponentIsNotCalled();

    MOCK_METHOD1( DeployConfiguration, int32_t( const PackageConfigInfo& ) );
    void MakeDeployConfigurationReturn( int32_t value );
    void ExpectDeployConfigurationIsNotCalled();

    MOCK_METHOD1( ResolvePath, std::string( const std::string& ) );
    void MakeResolvePathReturn( std::string value );
    void ExpectResolvePathIsNotCalled();

    MOCK_METHOD2( FileSearchWithWildCard, int32_t( const std::filesystem::path& searchPath, std::vector<std::filesystem::path>& results ) );
    void MakeFileSearchWithWildCardReturn( int32_t value );
    void ExpectFileSearchWithWildCardNotCalled();

    MOCK_METHOD0( InitiateSystemRestart, void() );
};

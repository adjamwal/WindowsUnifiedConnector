#pragma once
#include "MocksCommon.h"

#include "IPmPlatformComponentManager.h"

class MockPmPlatformComponentManager : public IPmPlatformComponentManager
{
public:
    MockPmPlatformComponentManager();
    ~MockPmPlatformComponentManager();

    MOCK_METHOD2( GetInstalledPackages, int32_t( PmInstalledPackage*, size_t& ) );
    void MakeGetInstalledPackagesReturn( int32_t value );
    void ExpectGetInstalledPackagesIsNotCalled();

    MOCK_METHOD1( InstallComponent, int32_t( const PmPackage& ) );
    void MakeInstallComponentReturn( int32_t value );
    void ExpectInstallComponentIsNotCalled();

    MOCK_METHOD1( UpdateComponent, int32_t( const PmPackage& ) );
    void MakeUpdateComponentReturn( int32_t value );
    void ExpectUpdateComponentIsNotCalled();

    MOCK_METHOD1( UninstallComponent, int32_t( const PmPackage& ) );
    void MakeUninstallComponentReturn( int32_t value );
    void ExpectUninstallComponentIsNotCalled();

    MOCK_METHOD1( DeployConfiguration, int32_t( const PmPackageConfigration& ) );
    void MakeDeployConfigurationReturn( int32_t value );
    void ExpectDeployConfigurationIsNotCalled();

};

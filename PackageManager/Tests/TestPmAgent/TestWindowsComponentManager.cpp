#include "MocksCommon.h"
#include "WindowsComponentManager.h"
#include "MockWinApiWrapper.h"
#include "MockCodesignVerifier.h"
#include "MockPackageDiscovery.h"
#include "PmLogger.h"

#include <memory>

class TestWindowsComponentManager : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_winApiWrapper.reset( new NiceMock<MockWinApiWrapper>() );
        m_codeSignVerifier.reset( new NiceMock<MockCodesignVerifier>() );
        m_packageDiscovery.reset( new NiceMock<MockPackageDiscovery>() );

        m_patient.reset( new WindowsComponentManager( *m_winApiWrapper, *m_codeSignVerifier, *m_packageDiscovery ) );
    }

    void TearDown()
    {
        m_patient.reset();

        m_winApiWrapper.reset();
        m_codeSignVerifier.reset();
        m_packageDiscovery.reset();

        m_expectedComponentPackage = {};
    }

    void SetupComponentPackage()
    {
        m_expectedComponentPackage = {
            "test/1.0.0",
            "installerUrl",
            "installerType",
            "installerArgs",
            "installLocation",
            "signerName",
            "installerHash",
            "installerPath",
            {}
        };

        m_expectedComponentPackage.configs.push_back( {
            "configpath",
            "configsha256",
            "configcontents",
            "configverifyBinPath",
            "configverifyPath",
            "installLocation",
            "signerName",
            "test/1.0.0",
            false
            } );
    }

    PmComponent m_expectedComponentPackage;
    std::unique_ptr<MockWinApiWrapper> m_winApiWrapper;
    std::unique_ptr<MockCodesignVerifier> m_codeSignVerifier;
    std::unique_ptr<MockPackageDiscovery> m_packageDiscovery;

    std::unique_ptr<WindowsComponentManager> m_patient;
};

TEST_F( TestWindowsComponentManager, WillGetInstalledPackages )
{
    EXPECT_CALL( *m_packageDiscovery, GetInstalledPackages( _ ) );

    std::vector<PmDiscoveryComponent> discoveryList;
    PackageInventory foundPackages;
    m_patient->GetInstalledPackages( discoveryList, foundPackages );
}

TEST_F( TestWindowsComponentManager, WillCodeSignVerifyOnUpdateComponent )
{
    SetupComponentPackage();
    EXPECT_CALL( *m_codeSignVerifier, Verify( _, _, _ ) );

    std::string error;
    m_patient->UpdateComponent( m_expectedComponentPackage, error );
}

TEST_F( TestWindowsComponentManager, WillCodeSignVerifyOnDeployConfiguration )
{
    EXPECT_CALL( *m_codeSignVerifier, Verify( _, _, _ ) );

    PackageConfigInfo config;
    m_patient->DeployConfiguration( config );
}

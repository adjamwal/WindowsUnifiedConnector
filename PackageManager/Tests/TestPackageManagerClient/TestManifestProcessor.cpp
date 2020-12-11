#include "gtest/gtest.h"
#include "ManifestProcessor.h"
#include "MockPmManifest.h"
#include "MockComponentPackageProcessor.h"
#include "MockPmPlatformDependencies.h"

#include <memory>

MATCHER_P( PmComponentMatch, expected, "" ) {
    return arg.packageNameAndVersion.compare( expected.packageNameAndVersion ) == 0 &&
        arg.installerUrl.compare( expected.installerUrl ) == 0 &&
        arg.installerType.compare( expected.installerType ) == 0 &&
        arg.installerArgs.compare( expected.installerArgs ) == 0 &&
        arg.installLocation.compare( expected.installLocation ) == 0 &&
        arg.signerName.compare( expected.signerName ) == 0 &&
        arg.installerHash.compare( expected.installerHash ) == 0 &&
        arg.installerPath.compare( expected.installerPath ) == 0;
}

class TestManifestProcessor : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_pmManifest.reset( new NiceMock<MockPmManifest>() );
        m_componentProcessor.reset( new NiceMock<MockComponentPackageProcessor>() );
        m_patient.reset( new ManifestProcessor( *m_pmManifest, *m_componentProcessor ) );
    }

    void TearDown()
    {
        m_patient.reset();
        m_componentProcessor.reset();
        m_pmManifest.reset();
        m_packageList = {};
        m_expectedComponentPackage = {};
    }

    void SetupPackageList( int componentCount )
    {
        m_expectedComponentPackage = {
                "packageName",
                "installerUrl",
                "installerType",
                "installerArgs",
                "installLocation",
                "signerName",
                "installerHash",
                "installerPath"
        };

        for( int i = 0; i < componentCount; i++ )
        {
            m_packageList.push_back( m_expectedComponentPackage );
        }

        m_pmManifest->MakeGetPackageListReturn( m_packageList );
        m_componentProcessor->MakeIsActionableReturn( true );
        m_componentProcessor->MakeHasConfigsReturn( true );
        m_componentProcessor->MakeProcessPackageBinariesReturn( true );
        m_componentProcessor->MakeProcessConfigsForPackageReturn( true );
    }

    std::vector<PmComponent> m_packageList;
    PmComponent m_expectedComponentPackage;
    std::unique_ptr<MockPmManifest> m_pmManifest;
    std::unique_ptr<MockComponentPackageProcessor> m_componentProcessor;
    std::unique_ptr<ManifestProcessor> m_patient;
};

TEST_F( TestManifestProcessor, WillInitializeDependency )
{
    std::unique_ptr<MockPmPlatformDependencies> dep;
    dep.reset( new NiceMock<MockPmPlatformDependencies>() );
    EXPECT_CALL( *m_componentProcessor, Initialize( dep.get() ) ).Times( 1 );

    m_patient->Initialize( dep.get() );
    dep.reset();
}

TEST_F( TestManifestProcessor, ProcessManifestWillParseManifest )
{
    std::string manifest = "test";
    EXPECT_CALL( *m_pmManifest, ParseManifest( manifest ) ).Times( 1 );

    m_patient->ProcessManifest( manifest );
}

TEST_F( TestManifestProcessor, ProcessManifestWillGetPackageList )
{
    EXPECT_CALL( *m_pmManifest, GetPackageList() ).Times( 1 );

    m_patient->ProcessManifest( "test" );
}

TEST_F( TestManifestProcessor, ProcessManifestWillProcessComponentPackage )
{
    SetupPackageList( 2 );
    EXPECT_CALL( *m_componentProcessor, 
        ProcessPackageBinaries( PmComponentMatch( m_expectedComponentPackage ) ) 
    ).Times( m_packageList.size() );

    m_patient->ProcessManifest( "test" );
}

TEST_F( TestManifestProcessor, ProcessManifestWillProcessAllPackagesRegardlessOfPackageFailures )
{
    SetupPackageList( 2 );
    EXPECT_CALL( *m_componentProcessor,
        ProcessPackageBinaries( PmComponentMatch( m_expectedComponentPackage ) )
    ).Times( m_packageList.size() );

    m_componentProcessor->MakeProcessPackageBinariesReturn( false );
    EXPECT_THROW( m_patient->ProcessManifest( "test" ), std::exception );
}

TEST_F( TestManifestProcessor, ProcessManifestWillThrowIfProcessComponentPackageFailed )
{
    SetupPackageList( 1 );
    m_componentProcessor->MakeProcessPackageBinariesReturn( false );
    EXPECT_THROW( m_patient->ProcessManifest( "test" ), std::exception );
}

TEST_F( TestManifestProcessor, ProcessManifestWillProcessConfigsForPackage )
{
    SetupPackageList( 2 );
    EXPECT_CALL( *m_componentProcessor,
        ProcessConfigsForPackage( PmComponentMatch( m_expectedComponentPackage ) )
    ).Times( m_packageList.size() );

    m_patient->ProcessManifest( "test" );
}

TEST_F( TestManifestProcessor, ProcessManifestWillProcessAllConfigsForSuccessfulPackagesRegardlessOfConfigFailures )
{
    SetupPackageList( 2 );
    EXPECT_CALL( *m_componentProcessor,
        ProcessConfigsForPackage( PmComponentMatch( m_expectedComponentPackage ) )
    ).Times( m_packageList.size() );

    m_componentProcessor->MakeProcessConfigsForPackageReturn( false );
    EXPECT_THROW( m_patient->ProcessManifest( "test" ), std::exception );
}

TEST_F( TestManifestProcessor, ProcessManifestWillThrowIfProcessConfigsForPackageFailed )
{
    SetupPackageList( 1 );
    m_componentProcessor->MakeProcessConfigsForPackageReturn( false );
    EXPECT_THROW( m_patient->ProcessManifest( "test" ), std::exception );
}


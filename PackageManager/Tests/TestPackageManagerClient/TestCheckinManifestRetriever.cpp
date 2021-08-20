#include "gtest/gtest.h"
#include "CheckinManifestRetriever.h"
#include "MockPmCloud.h"
#include "MockUcidAdapter.h"
#include "MockCertsAdapter.h"
#include "MockPmConfig.h"

#include <memory>

class TestCheckinManifestRetriever : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_cloud.reset( new NiceMock<MockPmCloud>() );
        m_ucidAdapter.reset( new NiceMock<MockUcidAdapter>() );
        m_certs.reset( new NiceMock<MockCertsAdapter>() );
        m_config.reset( new NiceMock<MockPmConfig>() );

        m_patient.reset( new CheckinManifestRetriever( *m_cloud, *m_ucidAdapter, *m_certs, *m_config ) );

        m_cloud->MakeCheckinReturn( true, "content", { 200, 0 } );
        m_config->MakeGetCloudCheckinUriReturn( "testuri" );
    }

    void TearDown()
    {
        m_patient.reset();
        m_cloud.reset();
        m_ucidAdapter.reset();
        m_certs.reset();
        m_config.reset();
    }

    std::unique_ptr<MockPmCloud> m_cloud;
    std::unique_ptr<MockUcidAdapter> m_ucidAdapter;
    std::unique_ptr<MockCertsAdapter> m_certs;
    std::unique_ptr<MockPmConfig> m_config;
    
    std::unique_ptr<CheckinManifestRetriever> m_patient;
};

TEST_F( TestCheckinManifestRetriever, WillSetUri )
{
    m_cloud->MakeCheckinReturn( true, "content", { 200, 0 } );
    
    m_ucidAdapter->MakeGetUcidTokenReturn( "token" );

    EXPECT_CALL( *m_cloud, SetCheckinUri( "testuri" ) ).Times( 1 );

    m_patient->GetCheckinManifest("testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillGetAccessToken )
{
    m_cloud->MakeCheckinReturn( true, "content", { 200, 0 } );
    m_ucidAdapter->MakeGetUcidTokenReturn( "token" );

    EXPECT_CALL( *m_ucidAdapter, GetAccessToken() ).Times( 1 );

    m_patient->GetCheckinManifest("testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillGetCertsList )
{
    m_cloud->MakeCheckinReturn( true, "content", { 200, 0 } );
    m_ucidAdapter->MakeGetUcidTokenReturn( "token" );

    EXPECT_CALL( *m_certs, GetCertsList() ).Times( 1 );

    m_patient->GetCheckinManifest("testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillSetToken )
{
    m_cloud->MakeCheckinReturn( true, "content", { 200, 0 } );
    m_ucidAdapter->MakeGetUcidTokenReturn( "token" );

    EXPECT_CALL( *m_cloud, SetToken( _ ) ).Times( 1 );

    m_patient->GetCheckinManifest("testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillSetCerts )
{
    m_cloud->MakeCheckinReturn( true, "content", { 200, 0 } );
    m_ucidAdapter->MakeGetUcidTokenReturn( "token" );

    EXPECT_CALL( *m_cloud, SetCerts( _ ) ).Times( 1 );

    m_patient->GetCheckinManifest("testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillCheckin )
{
    m_cloud->MakeCheckinReturn( true, "content", { 200, 0 } );
    m_ucidAdapter->MakeGetUcidTokenReturn( "token" );

    EXPECT_CALL( *m_cloud, Checkin( "testpayload", _, _ ) ).Times( 1 );

    m_patient->GetCheckinManifest("testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillThrowIfResponseIsNot200 )
{
    m_cloud->MakeCheckinReturn( false, "", { 404, 0 } );
    m_ucidAdapter->MakeGetUcidTokenReturn( "token" );

    EXPECT_THROW( m_patient->GetCheckinManifest("testpayload" ), std::exception );
}

TEST_F( TestCheckinManifestRetriever, WillGetUrlFromConfig )
{
    m_cloud->MakeCheckinReturn( true, "content", { 200, 0 } );
    m_ucidAdapter->MakeGetUcidTokenReturn( "token" );

    EXPECT_CALL( *m_config, GetCloudCheckinUri() ).Times( 1 );

    m_patient->GetCheckinManifest( "testpayload" );
}
TEST_F( TestCheckinManifestRetriever, WillRefreshTokenOnCheckinFailure )
{
    m_cloud->MakeCheckinReturn( false, "", { 401, 0 } );
    m_ucidAdapter->MakeGetUcidTokenReturn( "token" );

    EXPECT_CALL( *m_ucidAdapter, Refresh() ).Times( 1 );
    EXPECT_THROW( m_patient->GetCheckinManifest("testpayload" ), std::exception );
}

TEST_F( TestCheckinManifestRetriever, WillGetUrlAfterTokenIsRefreshed )
{
    m_cloud->MakeCheckinReturn( false, "", { 401, 0 } );
    m_ucidAdapter->MakeGetUcidTokenReturn( "token" );

    EXPECT_CALL( *m_config, GetCloudCheckinUri() ).Times( 2 );
    EXPECT_THROW( m_patient->GetCheckinManifest( "testpayload" ), std::exception );
}
TEST_F( TestCheckinManifestRetriever, WillNotRefreshTokenWhenTokenIsAuthorized )
{
    m_cloud->MakeCheckinReturn( false, "", { 404, 0 } );
    m_ucidAdapter->MakeGetUcidTokenReturn( "token" );

    m_ucidAdapter->ExpectRefreshIsNotCalled();
    EXPECT_THROW( m_patient->GetCheckinManifest("testpayload" ), std::exception );
}

TEST_F( TestCheckinManifestRetriever, WillNotCheckinWithoutToken )
{
    m_ucidAdapter->MakeGetUcidTokenReturn( "" );

    m_cloud->ExpectCheckinIsNotCalled();
    EXPECT_THROW( m_patient->GetCheckinManifest("testpayload" ), std::exception );
}

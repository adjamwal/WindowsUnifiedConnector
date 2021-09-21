#include "gtest/gtest.h"
#include "CatalogListRetriever.h"
#include "MockPmCloud.h"
#include "MockUcidAdapter.h"
#include "MockCertsAdapter.h"
#include "MockPmConfig.h"

#include <memory>

class TestCatalogListRetriever : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_cloud.reset( new NiceMock<MockPmCloud>() );
        m_ucidAdapter.reset( new NiceMock<MockUcidAdapter>() );
        m_certs.reset( new NiceMock<MockCertsAdapter>() );
        m_config.reset( new NiceMock<MockPmConfig>() );
        m_patient.reset( new CatalogListRetriever( *m_cloud, *m_ucidAdapter, *m_certs, *m_config ) );

        m_config->MakeGetCloudCatalogUriReturn( "uri" );
        m_ucidAdapter->MakeGetUcidTokenReturn( "token" );
        m_cloud->MakeGetReturn( true, "content", { 200, 0 } );
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
    std::unique_ptr<CatalogListRetriever> m_patient;
};

TEST_F( TestCatalogListRetriever, EventPublisherRetrievesTheCatalogUriFromConfig )
{
    EXPECT_CALL( *m_config, GetCloudCatalogUri() );
    m_patient->GetCloudCatalog();
}

TEST_F( TestCatalogListRetriever, WillGetAccessToken )
{
    EXPECT_CALL( *m_ucidAdapter, GetAccessToken() ).Times( 1 );
    m_patient->GetCloudCatalog();
}

TEST_F( TestCatalogListRetriever, WillSetToken )
{
    EXPECT_CALL( *m_cloud, SetToken( _ ) ).Times( 1 );
    m_patient->GetCloudCatalog();
}

TEST_F( TestCatalogListRetriever, WillCheckin )
{
    EXPECT_CALL( *m_cloud, Get( _, _, _ ) ).Times( 1 );
    m_patient->GetCloudCatalog();
}

TEST_F( TestCatalogListRetriever, WillThrowIfResponseIsNot200 )
{
    m_cloud->MakeGetReturn( false, "", { 404, 0 } );
    EXPECT_THROW( m_patient->GetCloudCatalog(), std::exception );
}

TEST_F( TestCatalogListRetriever, WillRefreshTokenOn401Response )
{
    m_cloud->MakeGetReturn( false, "", { 401, 0 } );
    EXPECT_CALL( *m_ucidAdapter, Refresh() ).Times( 1 );
    EXPECT_THROW( m_patient->GetCloudCatalog(), std::exception );
}

TEST_F( TestCatalogListRetriever, WillSkipTokenRefreshWhenNot401Response )
{
    m_cloud->MakeGetReturn( false, "", { 404, 0 } );
    m_ucidAdapter->ExpectRefreshIsNotCalled();
    EXPECT_THROW( m_patient->GetCloudCatalog(), std::exception );
}

TEST_F( TestCatalogListRetriever, WillNotHttpGetWithoutToken )
{
    m_ucidAdapter->MakeGetUcidTokenReturn( "" );

    m_cloud->ExpectGetIsNotCalled();
    EXPECT_THROW( m_patient->GetCloudCatalog(), std::exception );
}

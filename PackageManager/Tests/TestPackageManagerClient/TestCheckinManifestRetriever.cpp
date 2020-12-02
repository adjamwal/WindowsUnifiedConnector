#include "gtest/gtest.h"
#include "CheckinManifestRetriever.h"
#include "MockPmCloud.h"
#include "MockUcidAdapter.h"
#include "MockCertsAdapter.h"

#include <memory>

class TestCheckinManifestRetriever : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_cloud.reset( new NiceMock<MockPmCloud>() );
        m_token.reset( new NiceMock<MockUcidAdapter>() );
        m_certs.reset( new NiceMock<MockCertsAdapter>() );
        m_patient.reset( new CheckinManifestRetriever( *m_cloud, *m_token, *m_certs ) );
        m_cloud->MakeCheckinReturn( 200 );
    }

    void TearDown()
    {
        m_patient.reset();
        m_cloud.reset();
        m_token.reset();
        m_certs.reset();
    }

    std::unique_ptr<MockPmCloud> m_cloud;
    std::unique_ptr<MockUcidAdapter> m_token;
    std::unique_ptr<MockCertsAdapter> m_certs;
    std::unique_ptr<CheckinManifestRetriever> m_patient;
};

TEST_F( TestCheckinManifestRetriever, WillSetUri )
{
    m_cloud->MakeCheckinReturn( 200 );

    EXPECT_CALL( *m_cloud, SetUri( "testuri" ) ).Times( 1 );

    m_patient->GetCheckinManifestFrom( "testuri", "testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillGetAccessToken )
{
    m_cloud->MakeCheckinReturn( 200 );

    EXPECT_CALL( *m_token, GetAccessToken() ).Times( 1 );

    m_patient->GetCheckinManifestFrom( "testuri", "testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillGetCertsList )
{
    m_cloud->MakeCheckinReturn( 200 );

    EXPECT_CALL( *m_certs, GetCertsList() ).Times( 1 );

    m_patient->GetCheckinManifestFrom( "testuri", "testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillSetToken )
{
    m_cloud->MakeCheckinReturn( 200 );

    EXPECT_CALL( *m_cloud, SetToken( _ ) ).Times( 1 );

    m_patient->GetCheckinManifestFrom( "testuri", "testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillSetCerts )
{
    m_cloud->MakeCheckinReturn( 200 );

    EXPECT_CALL( *m_cloud, SetCerts( _ ) ).Times( 1 );

    m_patient->GetCheckinManifestFrom( "testuri", "testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillCheckin )
{
    m_cloud->MakeCheckinReturn( 200 );

    EXPECT_CALL( *m_cloud, Checkin( "testpayload", _ ) ).Times( 1 );

    m_patient->GetCheckinManifestFrom( "testuri", "testpayload" );
}

TEST_F( TestCheckinManifestRetriever, WillThrowIfResponseIsNot200 )
{
    m_cloud->MakeCheckinReturn( 404 );

    EXPECT_THROW( m_patient->GetCheckinManifestFrom( "testuri", "testpayload" ), std::exception );

}

TEST_F( TestCheckinManifestRetriever, WillRefreshTokenOnCheckinFailure )
{
    m_cloud->MakeCheckinReturn( 404 );

    EXPECT_CALL( *m_token, Refresh() ).Times( 1 );
    EXPECT_THROW( m_patient->GetCheckinManifestFrom( "testuri", "testpayload" ), std::exception );
}

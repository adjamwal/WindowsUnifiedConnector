#include "gtest/gtest.h"
#include "PmCloud.h"
#include "MockPmHttp.h"
#include "MockPmPlatformConfiguration.h"
#include "MockPmPlatformDependencies.h"

#include <memory>


class TestPmCloud : public ::testing::Test
{
protected:

    void SetUp()
    {
        m_eResult = {};
        m_http.reset( new NiceMock<MockPmHttp>() );
        m_mockPlatformConfiguration.reset( new NiceMock<MockPmPlatformConfiguration>() );
        m_mockDeps.reset( new NiceMock<MockPmPlatformDependencies>() );
        
        m_mockDeps->MakeConfigurationReturn( *m_mockPlatformConfiguration );
        m_patient.reset( new PmCloud( *m_http ) );
        m_patient->Initialize( m_mockDeps.get() );
    }

    void TearDown()
    {
        m_patient.reset();
        m_http.reset();
        m_mockDeps.reset();
        m_mockPlatformConfiguration.reset();
    }

    void SetupHttp()
    {
        m_http->MakeInitReturn( true, m_eResult );
        m_http->MakeSetCertsReturn( true, m_eResult );
        m_http->MakeSetTokenReturn( true, m_eResult );
    }

    PmHttpExtendedResult m_eResult;
    std::unique_ptr<MockPmHttp> m_http;
    std::unique_ptr<MockPmPlatformConfiguration> m_mockPlatformConfiguration;
    std::unique_ptr<MockPmPlatformDependencies> m_mockDeps;

    std::unique_ptr<PmCloud> m_patient;
};

TEST_F( TestPmCloud, GetWillSucceed )
{
    PmHttpExtendedResult eResult;
    std::string response;
    SetupHttp();

    m_eResult.httpResponseCode = 200;
    m_http->MakeHttpGetReturn( true, "", m_eResult);

    EXPECT_TRUE( m_patient->Get( "http://url.com", response, eResult ) );
}

TEST_F( TestPmCloud, PostWillSucceed )
{
    PmHttpExtendedResult eResult;
    std::string response;
    SetupHttp();

    m_eResult.httpResponseCode = 200;
    m_http->MakeHttpPostReturn( true, "", m_eResult );

    EXPECT_TRUE( m_patient->Post( "http://url.com", NULL, 0, response, eResult ) );
}

TEST_F( TestPmCloud, DownloadFileWillSucceed )
{
    PmHttpExtendedResult eResult;
    SetupHttp();

    m_eResult.httpResponseCode = 200;
    m_http->MakeHttpDownloadReturn( true, m_eResult );

    EXPECT_TRUE( m_patient->DownloadFile( "http://url.com", "file.txt", eResult ) );
}

TEST_F( TestPmCloud, GetWillUpdateCertOnPeerValidationFailure )
{
    PmHttpExtendedResult eResult;
    std::string response;
    SetupHttp();

    m_http->MakeHttpGetReturn( false, "", m_eResult );
    m_http->MakeIsSslPeerValidationErrorReturn( true );

    EXPECT_CALL( *m_mockPlatformConfiguration, UpdateCertStoreForUrl( "http://url.com" ) );

    m_patient->Get( "http://url.com", response, eResult );
}

TEST_F( TestPmCloud, PostWillUpdateCertOnPeerValidationFailure )
{
    PmHttpExtendedResult eResult;
    std::string response;
    SetupHttp();

    m_http->MakeHttpPostReturn( false, "", m_eResult );
    m_http->MakeIsSslPeerValidationErrorReturn( true );

    EXPECT_CALL( *m_mockPlatformConfiguration, UpdateCertStoreForUrl( "http://url.com" ) );

    m_patient->Post( "http://url.com", NULL, 0, response, eResult );
}

TEST_F( TestPmCloud, DownloadFileWillUpdateCertOnPeerValidationFailure )
{
    PmHttpExtendedResult eResult;
    SetupHttp();

    m_http->MakeHttpDownloadReturn( false,  m_eResult );
    m_http->MakeIsSslPeerValidationErrorReturn( true );

    EXPECT_CALL( *m_mockPlatformConfiguration, UpdateCertStoreForUrl( "http://url.com" ) );

    m_patient->DownloadFile( "http://url.com", "file.txt", eResult );
}
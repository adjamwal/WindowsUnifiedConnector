#include "gtest/gtest.h"
#include "PmHttp.h"
#include "MockFileUtil.h"
#include "fff.h"

#include <memory>

DEFINE_FFF_GLOBALS

FAKE_VALUE_FUNC( const char*, curl_easy_strerror, CURLcode );
FAKE_VALUE_FUNC( CURL*, curl_easy_init );
FAKE_VALUE_FUNC_VARARG( CURLcode, curl_easy_setopt, CURL*, CURLoption, ... );
FAKE_VALUE_FUNC( CURLcode, curl_easy_perform, CURL* );
FAKE_VOID_FUNC( curl_easy_cleanup, CURL* );
FAKE_VALUE_FUNC_VARARG( CURLcode, curl_easy_getinfo, CURL*, CURLINFO, ... );

FAKE_VALUE_FUNC( int, xferCallback, void*, PM_TYPEOF_OFF_T, PM_TYPEOF_OFF_T, PM_TYPEOF_OFF_T, PM_TYPEOF_OFF_T );

class TestPmHttp: public ::testing::Test
{
protected:
    void ResetFakes()
    {
        RESET_FAKE( curl_easy_strerror );
        RESET_FAKE( curl_easy_init );
        RESET_FAKE( curl_easy_setopt );
        RESET_FAKE( curl_easy_perform );
        RESET_FAKE( curl_easy_cleanup );
        RESET_FAKE( curl_easy_getinfo );
        RESET_FAKE( xferCallback );
        FFF_RESET_HISTORY();
    }

    void SetUp()
    {
        ResetFakes();
        m_fileUtil.reset( new NiceMock<MockFileUtil>() );

        m_patient.reset( new PmHttp( *m_fileUtil ) );
    }

    void TearDown()
    {
        m_patient.reset();

        m_fileUtil.reset();
    }

    bool FindCurlOpt( CURLoption option ) 
    {
        for( int i = 0; i < curl_easy_setopt_fake.call_count; i++ ) {
            if( curl_easy_setopt_fake.arg1_history[ i ] == option ) {
                return true;
            }
        }

        return false;
    }

    void InitPatient()
    {
        curl_easy_init_fake.return_val = ( CURL* )1;
        m_patient->Init( NULL, NULL, "" );
        ResetFakes();
    }

    std::unique_ptr<MockFileUtil> m_fileUtil;
    std::unique_ptr<PmHttp> m_patient;
};

TEST_F( TestPmHttp, WillOnlyInitOnce )
{
    curl_easy_init_fake.return_val = ( CURL* )1;

    m_patient->Init( NULL, NULL, "" );
    m_patient->Init( NULL, NULL, "" );

    EXPECT_EQ( curl_easy_init_fake.call_count, 1 );
}

TEST_F( TestPmHttp, InitWillNotSetCallback )
{
    curl_easy_init_fake.return_val = ( CURL* )1;

    m_patient->Init( NULL, (void*)1, "" );

    EXPECT_FALSE( FindCurlOpt( CURLOPT_XFERINFOFUNCTION ) );
    EXPECT_FALSE( FindCurlOpt( CURLOPT_XFERINFODATA ) );
}

TEST_F( TestPmHttp, InitWillSetCallback )
{
    curl_easy_init_fake.return_val = ( CURL* )1;

    m_patient->Init( xferCallback, NULL, "" );

    EXPECT_TRUE( FindCurlOpt( CURLOPT_XFERINFOFUNCTION ) );
    EXPECT_TRUE( FindCurlOpt( CURLOPT_XFERINFODATA ) );
}

TEST_F( TestPmHttp, InitWillSucceed )
{
    curl_easy_init_fake.return_val = ( CURL* )1;

    EXPECT_EQ( m_patient->Init( NULL, NULL, "" ), CURLE_OK );
}

TEST_F( TestPmHttp, InitWillFail )
{
    curl_easy_init_fake.return_val = ( CURL* )1;
    curl_easy_setopt_fake.return_val = CURL_LAST;

    EXPECT_EQ( m_patient->Init( NULL, NULL, "" ), curl_easy_setopt_fake.return_val );
}

TEST_F( TestPmHttp, DeInitWillSucceed )
{
    EXPECT_EQ( m_patient->Deinit(), CURLE_OK );
}

TEST_F( TestPmHttp, DeinitWillCleanup )
{
    InitPatient();

    m_patient->Deinit();
    EXPECT_EQ( curl_easy_cleanup_fake.call_count, 1 );
}

TEST_F( TestPmHttp, HttpGetFailsWhenNotInitialize )
{
    std::string response;
    int32_t httpRtn = 0;

    EXPECT_NE( m_patient->HttpGet( "http://", response, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpGetFailsWithEmptyURL )
{
    std::string response;
    int32_t httpRtn = 0;

    InitPatient();

    EXPECT_NE( m_patient->HttpGet( "", response, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpGetWillRun )
{
    std::string response;
    int32_t httpRtn = 0;

    InitPatient();

    EXPECT_EQ( m_patient->HttpGet( "https://", response, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_perform_fake.call_count, 1 );
}

TEST_F( TestPmHttp, HttpPostFailsWhenNotInitialize )
{
    std::string response;
    int32_t httpRtn = 0;
    int32_t data = 0;

    EXPECT_NE( m_patient->HttpPost( "http://", &data, sizeof(data), response, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpPostFailsWithEmptyURL )
{
    std::string response;
    int32_t httpRtn = 0;
    int32_t data = 0;

    InitPatient();

    EXPECT_NE( m_patient->HttpPost( "", &data, sizeof( data ), response, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpPostFailsWithInvalidData )
{
    std::string response;
    int32_t httpRtn = 0;
    int32_t data = 0;

    InitPatient();

    EXPECT_NE( m_patient->HttpPost( "http://", NULL, sizeof( data ), response, httpRtn ), CURLE_OK );
    EXPECT_NE( m_patient->HttpPost( "http://", &data, 0, response, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpPostWillRun )
{
    std::string response;
    int32_t httpRtn = 0;
    int32_t data = 0;

    InitPatient();

    EXPECT_EQ( m_patient->HttpPost( "http://", &data, sizeof( data ), response, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_perform_fake.call_count, 1 );
}

TEST_F( TestPmHttp, HttpDownlaodFailsWhenNotInitialize )
{
    std::string filepath = "FilePath";
    int32_t httpRtn = 0;
    
    EXPECT_NE( m_patient->HttpDownload( "http://", filepath, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpDownloadFailsWithEmptyURL )
{
    std::string filepath = "FilePath";
    int32_t httpRtn = 0;

    InitPatient();

    EXPECT_NE( m_patient->HttpDownload( "", filepath, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpDownloadFailsWithInvalidFilePath )
{
    std::string filepath;
    int32_t httpRtn = 0;

    InitPatient();

    EXPECT_NE( m_patient->HttpDownload( "http://", filepath, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpDownloadFailsWhenFileIsntCreated )
{
    std::string filepath;
    int32_t httpRtn = 0;

    InitPatient();
    m_fileUtil->MakePmCreateFileReturn( NULL );

    EXPECT_NE( m_patient->HttpDownload( "http://", filepath, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpDownloadWillRun )
{
    std::string filepath = "FilePath";
    int32_t httpRtn = 0;

    InitPatient();
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );

    EXPECT_EQ( m_patient->HttpDownload( "http://", filepath, httpRtn ), CURLE_OK );
    EXPECT_EQ( curl_easy_perform_fake.call_count, 1 );
}


TEST_F( TestPmHttp, HttpDownloadWillCloseFile )
{
    std::string filepath = "FilePath";
    int32_t httpRtn = 0;

    InitPatient();
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );

    EXPECT_CALL( *m_fileUtil, CloseFile( ( FileUtilHandle* )1 ) );

    m_patient->HttpDownload( "http://", filepath, httpRtn );
}
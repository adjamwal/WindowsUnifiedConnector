#include "gtest/gtest.h"
#include "PmHttp.h"
#include "MockFileSysUtil.h"
#include "fff.h"

#include <memory>

#ifdef X509_NAME
#undef X509_NAME
#endif

DEFINE_FFF_GLOBALS

extern "C" {
    FAKE_VALUE_FUNC( const char*, curl_easy_strerror, CURLcode );
    FAKE_VALUE_FUNC( CURL*, curl_easy_init );
    FAKE_VALUE_FUNC_VARARG( CURLcode, curl_easy_setopt, CURL*, CURLoption, ... );
    FAKE_VALUE_FUNC( CURLcode, curl_easy_perform, CURL* );
    FAKE_VOID_FUNC( curl_easy_cleanup, CURL* );
    FAKE_VALUE_FUNC_VARARG( CURLcode, curl_easy_getinfo, CURL*, CURLINFO, ... );
    FAKE_VOID_FUNC( curl_slist_free_all, struct curl_slist* );
    FAKE_VALUE_FUNC( struct curl_slist*, curl_slist_append, struct curl_slist*, const char* );
    FAKE_VALUE_FUNC( int, xferCallback, void*, PM_TYPEOF_OFF_T, PM_TYPEOF_OFF_T, PM_TYPEOF_OFF_T, PM_TYPEOF_OFF_T );

    FAKE_VALUE_FUNC( BIO*, BIO_new, const BIO_METHOD* );
    FAKE_VALUE_FUNC( int, BIO_free, BIO* );
    FAKE_VALUE_FUNC( long, BIO_ctrl, BIO*, int, long, void* );
    FAKE_VALUE_FUNC( const BIO_METHOD*, BIO_s_mem );
    FAKE_VALUE_FUNC( int, X509_STORE_add_cert, X509_STORE*, X509* );
    FAKE_VALUE_FUNC( int, X509_NAME_print_ex, BIO*, const X509_NAME*, int, unsigned long );
    FAKE_VALUE_FUNC( X509_NAME*, X509_get_subject_name, const X509* );
    FAKE_VOID_FUNC( X509_free, X509* );
    FAKE_VALUE_FUNC( int, X509_up_ref, X509* );

    FAKE_VALUE_FUNC( X509_STORE*, SSL_CTX_get_cert_store, const SSL_CTX* );
    FAKE_VALUE_FUNC( unsigned long, ERR_get_error );
    FAKE_VALUE_FUNC( char*, ERR_error_string, unsigned long, char* );
}

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
        RESET_FAKE( curl_slist_free_all );
        RESET_FAKE( curl_slist_append );
        RESET_FAKE( xferCallback );

        RESET_FAKE( BIO_new );
        RESET_FAKE( BIO_free );
        RESET_FAKE( BIO_ctrl );
        RESET_FAKE( BIO_s_mem );
        RESET_FAKE( X509_STORE_add_cert );
        RESET_FAKE( X509_NAME_print_ex );
        RESET_FAKE( X509_get_subject_name );
        RESET_FAKE( X509_free );
        RESET_FAKE( X509_up_ref );
        RESET_FAKE( SSL_CTX_get_cert_store );
        RESET_FAKE( ERR_get_error );
        RESET_FAKE( ERR_error_string );
        FFF_RESET_HISTORY();
    }

    void SetUp()
    {
        ResetFakes();
        m_fileUtil.reset( new NiceMock<MockFileSysUtil>() );
        m_eResult = {};
        m_patient.reset( new PmHttp( *m_fileUtil ) );
    }

    void TearDown()
    {
        m_patient.reset();
        m_eResult = {};
        m_fileUtil.reset();
    }

    bool FindCurlOpt( CURLoption option ) 
    {
        for( unsigned int i = 0; i < curl_easy_setopt_fake.call_count; i++ ) {
            if( curl_easy_setopt_fake.arg1_history[ i ] == option ) {
                return true;
            }
        }

        return false;
    }

    void InitPatient()
    {
        curl_easy_init_fake.return_val = ( CURL* )1;
        m_patient->Init( NULL, NULL, "", m_eResult );
        ResetFakes();
    }

    PmHttpExtendedResult m_eResult;
    std::unique_ptr<MockFileSysUtil> m_fileUtil;
    std::unique_ptr<PmHttp> m_patient;
};

TEST_F( TestPmHttp, WillOnlyInitOnce )
{
    curl_easy_init_fake.return_val = ( CURL* )1;

    m_patient->Init( NULL, NULL, "", m_eResult );
    m_patient->Init( NULL, NULL, "", m_eResult );

    EXPECT_EQ( curl_easy_init_fake.call_count, 1 );
}

TEST_F( TestPmHttp, InitWillNotSetCallback )
{
    curl_easy_init_fake.return_val = ( CURL* )1;

    m_patient->Init( NULL, (void*)1, "", m_eResult );

    EXPECT_FALSE( FindCurlOpt( CURLOPT_XFERINFOFUNCTION ) );
    EXPECT_FALSE( FindCurlOpt( CURLOPT_XFERINFODATA ) );
}

TEST_F( TestPmHttp, InitWillSetCallback )
{
    curl_easy_init_fake.return_val = ( CURL* )1;

    m_patient->Init( xferCallback, NULL, "", m_eResult );

    EXPECT_TRUE( FindCurlOpt( CURLOPT_XFERINFOFUNCTION ) );
    EXPECT_TRUE( FindCurlOpt( CURLOPT_XFERINFODATA ) );
}

TEST_F( TestPmHttp, InitWillSucceed )
{
    curl_easy_init_fake.return_val = ( CURL* )1;

    EXPECT_EQ( m_patient->Init( NULL, NULL, "", m_eResult ), true );
}

TEST_F( TestPmHttp, InitWillFail )
{
    curl_easy_init_fake.return_val = ( CURL* )1;
    curl_easy_setopt_fake.return_val = CURL_LAST;

    m_patient->Init( NULL, NULL, "", m_eResult );

    EXPECT_EQ( m_eResult.subErrorCode, curl_easy_setopt_fake.return_val );
}

TEST_F( TestPmHttp, DeInitWillSucceed )
{
    InitPatient();
    EXPECT_EQ( m_patient->Deinit(), true );
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

    EXPECT_NE( m_patient->HttpGet( "http://", response, m_eResult ), true );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpGetFailsWithEmptyURL )
{
    std::string response;

    InitPatient();

    EXPECT_NE( m_patient->HttpGet( "", response, m_eResult ), true );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpGetWillRun )
{
    std::string response;

    InitPatient();

    EXPECT_EQ( m_patient->HttpGet( "https://", response, m_eResult ), true );
    EXPECT_EQ( curl_easy_perform_fake.call_count, 1 );
}

TEST_F( TestPmHttp, HttpPostFailsWhenNotInitialize )
{
    std::string response;
    
    int32_t data = 0;

    EXPECT_NE( m_patient->HttpPost( "http://", &data, sizeof(data), response, m_eResult ), true );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpPostFailsWithEmptyURL )
{
    std::string response;
    
    int32_t data = 0;

    InitPatient();

    EXPECT_NE( m_patient->HttpPost( "", &data, sizeof( data ), response, m_eResult ), true );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpPostFailsWithInvalidData )
{
    std::string response;
    
    int32_t data = 0;

    InitPatient();

    EXPECT_NE( m_patient->HttpPost( "http://", NULL, sizeof( data ), response, m_eResult ), true );
    EXPECT_NE( m_patient->HttpPost( "http://", &data, 0, response, m_eResult ), true );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpPostWillRun )
{
    std::string response;
    int32_t data = 0;

    InitPatient();

    EXPECT_EQ( m_patient->HttpPost( "http://", &data, sizeof( data ), response, m_eResult ), true );
    EXPECT_EQ( curl_easy_perform_fake.call_count, 1 );
}

TEST_F( TestPmHttp, HttpDownloadFailsWhenNotInitialized )
{
    std::string filepath = "FilePath";
    
    EXPECT_NE( m_patient->HttpDownload( "http://", filepath, m_eResult ), true );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpDownloadFailsWithEmptyURL )
{
    std::string filepath = "FilePath";

    InitPatient();

    EXPECT_NE( m_patient->HttpDownload( "", filepath, m_eResult ), true );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpDownloadFailsWithInvalidFilePath )
{
    std::string filepath;

    InitPatient();

    EXPECT_NE( m_patient->HttpDownload( "http://", filepath, m_eResult ), true );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpDownloadFailsWhenFileIsntCreated )
{
    std::string filepath;

    InitPatient();
    m_fileUtil->MakePmCreateFileReturn( NULL );

    EXPECT_NE( m_patient->HttpDownload( "http://", filepath, m_eResult ), true );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, HttpDownloadWillRun )
{
    std::string filepath = "FilePath";
    

    InitPatient();
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );

    EXPECT_EQ( m_patient->HttpDownload( "http://", filepath, m_eResult ), true );
    EXPECT_EQ( curl_easy_perform_fake.call_count, 1 );
}


TEST_F( TestPmHttp, HttpDownloadWillCloseFile )
{
    std::string filepath = "FilePath";
    

    InitPatient();
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );

    EXPECT_CALL( *m_fileUtil, CloseFile( ( FileUtilHandle* )1 ) );

    m_patient->HttpDownload( "http://", filepath, m_eResult );
}

TEST_F( TestPmHttp, SetTokenFailsWhenNotInitialize )
{
    std::string token( "MyToken" );

    EXPECT_NE( m_patient->SetToken( token, m_eResult ), true );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, SetTokenWillSucceed )
{
    std::string token( "MyToken" );
    

    InitPatient();
    curl_slist_append_fake.return_val = ( struct curl_slist* )1;

    EXPECT_EQ( m_patient->SetToken( token, m_eResult ), true );
}

TEST_F( TestPmHttp, SetTokenWillUpdateHeader )
{
    std::string token( "MyToken" );
    

    InitPatient();
    curl_slist_append_fake.return_val = ( struct curl_slist* )1;

    m_patient->SetToken( token, m_eResult );

    EXPECT_NE( std::string( curl_slist_append_fake.arg1_val ).find( token ), std::string::npos );
    EXPECT_TRUE( FindCurlOpt( CURLOPT_HTTPHEADER ) );
}


TEST_F( TestPmHttp, SetTokenFreePreviousToken )
{
    std::string token( "MyToken" );
    

    InitPatient();
    curl_slist_append_fake.return_val = ( struct curl_slist* )1;

    m_patient->SetToken( token, m_eResult );
    m_patient->SetToken( token, m_eResult );
    
    EXPECT_EQ( curl_slist_free_all_fake.call_count, 1 );
}

TEST_F( TestPmHttp, SetCertsFailsWhenNotInitialize )
{
    PmHttpCertList certs{};

    EXPECT_NE( m_patient->SetCerts( certs, m_eResult ), true );
    EXPECT_EQ( curl_easy_setopt_fake.call_count, 0 );
}

TEST_F( TestPmHttp, SetCertWillSucceed )
{
    PmHttpCertList certs{};

    InitPatient();
    curl_slist_append_fake.return_val = ( struct curl_slist* )1;

    EXPECT_EQ( m_patient->SetCerts( certs, m_eResult ), true );
}

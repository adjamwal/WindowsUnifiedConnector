#include "gtest/gtest.h"
#include "PmHttp.h"
#include "MockFileUtil.h"

#include <memory>
#include "WindowsCertStore.h"

class ComponentTestPmHttp : public ::testing::Test
{
protected:
    ComponentTestPmHttp() :
        m_certList( { 0 } )
    {
#if defined ( WIN32 )
        // On windows we need to pass the certs from the windows cert store to curl
        // This can be removed on curl 7.71, however the feature that does this is still in beta
        cert_store_init();
        cert_store_load( NULL, 0 );
        cert_store_retrieve_certs( &m_certList.certificates, &m_certList.count );
#endif
    }

    ~ComponentTestPmHttp()
    {
        cert_store_unload();
    }

    void SetUp()
    {
        m_fileUtil.reset( new NiceMock<MockFileUtil>() );

        m_patient.reset( new PmHttp( *m_fileUtil ) );
    }

    void TearDown()
    {
        m_patient.reset();

        m_fileUtil.reset();
    }

    PmHttpCertList m_certList;
    std::unique_ptr<MockFileUtil> m_fileUtil;
    std::unique_ptr<PmHttp> m_patient;
};

TEST_F( ComponentTestPmHttp, GetTest )
{
    std::string response;
    int32_t rtn = 0;

    m_patient->Init( NULL, NULL, "" );
    m_patient->SetCerts( m_certList );

    m_patient->HttpGet( "https://postman-echo.com/get?foo1=bar1&foo2=bar2", response, rtn );
    EXPECT_EQ( rtn, 200 );
}

TEST_F( ComponentTestPmHttp, PostTest )
{
    std::string response;
    std::string data( "{\"args\":{\"foo1\":\"bar1\",\"foo2\":\"bar2\"}}" );
    int32_t rtn = 0;

    m_patient->Init( NULL, NULL, "" );
    m_patient->SetCerts( m_certList );
    
    m_patient->HttpPost( "https://postman-echo.com/post", ( void* )data.c_str(), data.length(), response, rtn );

    EXPECT_EQ( rtn, 200 );
}

TEST_F( ComponentTestPmHttp, DownloadTest )
{
    std::string path( "filepath" );
    int32_t rtn = 0;

    m_patient->Init( NULL, NULL, "" );
    m_patient->SetCerts( m_certList );

    InSequence s;
    EXPECT_CALL( *m_fileUtil, PmCreateFile( _ ) ).WillOnce( Return( ( FileUtilHandle* )1) );
    ON_CALL( *m_fileUtil, AppendFile( _, _, _ ) ).WillByDefault( Invoke( []( FileUtilHandle* handle, void* data, size_t dataLen )
        {
            return dataLen;
        } ) );
    EXPECT_CALL( *m_fileUtil, CloseFile( _ ) );

    m_patient->HttpDownload( "https://postman-echo.com/get", path, rtn );

    EXPECT_EQ( rtn, 200 );
}

TEST_F( ComponentTestPmHttp, HttpPostHttpProtocalIsNotValid )
{
    std::string response;
    std::string data( "{\"args\":{\"foo1\":\"bar1\",\"foo2\":\"bar2\"}}" );
    int32_t rtn = 0;
    int32_t methodRtn = 0;

    m_patient->Init( NULL, NULL, "" );
    m_patient->SetCerts( m_certList );

    methodRtn = m_patient->HttpPost( "http://postman-echo.com/post", (void*)data.c_str(), data.length(), response, rtn );

    EXPECT_NE( methodRtn, 0 );
}

TEST_F( ComponentTestPmHttp, HttpPostBlankProtocalIsNotValid )
{
    std::string response;
    std::string data( "{\"args\":{\"foo1\":\"bar1\",\"foo2\":\"bar2\"}}" );
    int32_t rtn = 0;
    int32_t methodRtn = 0;

    m_patient->Init( NULL, NULL, "" );
    m_patient->SetCerts( m_certList );

    methodRtn = m_patient->HttpPost( "postman-echo.com/post", (void*)data.c_str(), data.length(), response, rtn );

    EXPECT_NE( methodRtn, 0 );
}

TEST_F( ComponentTestPmHttp, GetWillClearResponse)
{
    std::string badText = "Invalid Text";
    std::string response = badText;
    int32_t rtn = 0;

    m_patient->Init( NULL, NULL, "" );
    m_patient->SetCerts( m_certList );

    m_patient->HttpGet( "https://postman-echo.com/get?foo1=bar1&foo2=bar2", response, rtn );

    EXPECT_NE( response.find( badText ), 0 );
}

TEST_F( ComponentTestPmHttp, PostWillClearResponse )
{
    std::string badText = "Invalid Text";
    std::string response = badText;
    std::string data( "{\"args\":{\"foo1\":\"bar1\",\"foo2\":\"bar2\"}}" );
    int32_t rtn = 0;

    m_patient->Init( NULL, NULL, "" );
    m_patient->SetCerts( m_certList );

    m_patient->HttpPost( "https://postman-echo.com/post", ( void* )data.c_str(), data.length(), response, rtn );

    EXPECT_NE( response.find( badText ), 0 );
}
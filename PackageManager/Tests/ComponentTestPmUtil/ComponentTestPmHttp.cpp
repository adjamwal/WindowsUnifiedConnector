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

TEST_F( ComponentTestPmHttp, DISABLED__CanPostToCheckinURL )
{
    std::string response;
    int32_t rtn = 0;

    m_patient->Init( NULL, NULL, "" );
    m_patient->SetCerts( m_certList );
    m_patient->SetToken( "v2.public.eyJVQ0lEIjoiNDI2Y2JjMjYtOTlmMS00ZTA1LTgzMTYtMmVkN2YwODk4OGYyIiwiYmlkIjoiZGVtby1idXMtaWQiLCJleHAiOiIyMDIwLTEwLTE2VDE3OjUxOjEzWiIsImlhdCI6IjIwMjAtMTAtMTZUMTc6NDY6MTNaIiwiaXNzIjoiQ2lzY28gSWRlbnRpdHkgU2VydmljZXMiLCJqdGkiOiIxNjAyODcwMzczLjE2MDI4NzAzNzMxNzc4OTgxNjYuMTExMTI4MDc2MTQ3MjYyODMiLCJuYmYiOiIyMDIwLTEwLTE2VDE3OjQ2OjEzWiJ9XuVulBsLgElQyA4S6NL4RuG7_6uKzkiiNyYa5jbSYx-rRQ4dPlsTLd5IFes1RzFUm7PVwxwJdeF5QfgY4BGGAg.eyJraWQiOiJrZXktZCJ9" );
    
    // TODO: Will need to change this to the real thing
    m_patient->HttpPost( "https://z0w2hzsjcg.execute-api.us-west-1.amazonaws.com/dev/checkin", ( void* )"{}", 3, response, rtn );

    // Expect 403 because the token has expired
    EXPECT_EQ( rtn, 403 );
}

TEST_F( ComponentTestPmHttp, GetTest )
{
    std::string response;
    int32_t rtn = 0;

    m_patient->Init( NULL, NULL, "" );
    m_patient->SetCerts( m_certList );

    m_patient->HttpGet( "https://postman-echo.com/get?foo1=bar1&foo2=bar2", response, rtn );
    printf( "%s\n", response.c_str() );
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

    printf( "%s\n", response.c_str() );
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
            if( data && dataLen ) {
                printf( "%s\n", ( const char* )data );
            }
            return dataLen;
        } ) );
    EXPECT_CALL( *m_fileUtil, CloseFile( _ ) );

    m_patient->HttpDownload( "https://postman-echo.com/get", path, rtn );

    EXPECT_EQ( rtn, 200 );
}

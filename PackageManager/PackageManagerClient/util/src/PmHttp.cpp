#include "PmHttp.h"
#include "PmLogger.h"
#include "IFileUtil.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

#ifdef WIN32
// X509_NAME is defined as a constant in wincrypt.h
#ifdef X509_NAME
#undef X509_NAME
#endif
#endif

size_t PmHttp::WriteString( void* ptr, size_t size, size_t nmemb, std::string* data )
{
    if( data ) {
        data->append( ( char* )ptr, size * nmemb );
    }
    return size * nmemb;
}

size_t PmHttp::WriteFile( void* ptr, size_t size, size_t nmemb, WriteFileCtx* data )
{
    if( data && data->fileUtil ) {
        data->fileUtil->AppendFile( data->handle, ptr, size * nmemb );
    }
    return size * nmemb;
}

int PmHttp::X509_subj_name( const X509* cert, char** subj_name ) 
{
    int rc = -1;

    if( !cert || !subj_name ) {
        WLOG_ERROR( L"Invalid parameters" );
        return rc;
    }

    X509_NAME* cert_subject_name = X509_get_subject_name( cert );
    BIO* bio_out = NULL;
    BUF_MEM* biomem;

    *subj_name = NULL;

    bio_out = BIO_new( BIO_s_mem() );
    if( !bio_out ) {
        WLOG_ERROR( L"BIO_new failed" );
    }
    else if( -1 == X509_NAME_print_ex( bio_out, cert_subject_name, 0, XN_FLAG_SEP_SPLUS_SPC ) ) {
        WLOG_ERROR( L"X509_NAME_print_ex failed" );
    }
    else {
        BIO_get_mem_ptr( bio_out, &biomem );

        *subj_name = ( char* )calloc( 1, biomem->length + 1 );
        if( *subj_name ) {
            memcpy( *subj_name, biomem->data, biomem->length );
            *subj_name[ biomem->length ] = '\0';

            rc = 0;
        }
    }
    
    if( NULL != bio_out ) {
        BIO_free( bio_out );
    }

    return rc;
}

CURLcode PmHttp::SslCallback( CURL* curl, void* sslctx, void* param )
{
    PmHttpCertList *certList = ( PmHttpCertList* )param;
    CURLcode cb_ret = CURLE_ABORTED_BY_CALLBACK;

    ( void )curl; /* UNUSED */

    X509_STORE* store;

    if( ( sslctx == NULL ) || ( param == NULL ) ) {
        WLOG_ERROR( L"Invalid parameters" );
        return cb_ret;
    }

    /* Add CA certs from client trust to X509_STORE */
    if( ( store = SSL_CTX_get_cert_store( ( SSL_CTX* )sslctx ) ) != NULL ) {
        size_t i;
        for( i = 0; i < certList->count; ++i ) {
            if( X509_STORE_add_cert( store, certList->certificates[ i ] ) != 1 ) {
                unsigned long err = ERR_get_error();
                char* trusted_root_cert_name = NULL;
                if( X509_subj_name( certList->certificates[ i ], &trusted_root_cert_name ) != 0 ) {
                    trusted_root_cert_name = NULL;
                }
                if( X509_R_CERT_ALREADY_IN_HASH_TABLE == ERR_GET_REASON( err ) ) {
                    LOG_DEBUG( "Certificate add skipped, certificate is already in trust: %s",
                        ( trusted_root_cert_name ? trusted_root_cert_name : "" ) );
                }
                else {
                    LOG_ERROR( "Cannot store certificate to trust: %s (%lu) [%s]",
                        ERR_error_string( err, NULL ), err,
                        ( trusted_root_cert_name ? trusted_root_cert_name : "" ) );
                }
                free( trusted_root_cert_name );
                trusted_root_cert_name = NULL;
            }
        }
    }

    cb_ret = CURLE_OK;
    return cb_ret;
}

PmHttp::PmHttp( IFileUtil& fileUtil ) :
    m_fileUtil( fileUtil )
    , m_curlHandle( nullptr )
    , m_agent( "DefaultPackageManager" )
    , m_headerList( nullptr )
    , m_certList( { 0 } )
{

}

PmHttp::~PmHttp()
{
    if( m_headerList ) {
        curl_slist_free_all( m_headerList );
        m_headerList = nullptr;
    }

    FreeCerts();
}

int32_t PmHttp::Init( PM_PROGRESS_CALLBACK callback, void* ctx, const std::string& agent )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    CURLcode rtn = CURLE_OK;

    if( m_curlHandle ) {
        WLOG_ERROR( L"PmHttp Instance already initialized" );
        return rtn;
    }

    m_curlHandle = curl_easy_init();
    if( !m_curlHandle ) {
        WLOG_ERROR( L"curl_easy_init failed" );
        return -1;
    }

    if( !agent.empty() ) {
        m_agent = agent;
    }

    rtn = curl_easy_setopt( m_curlHandle, CURLOPT_USERAGENT, m_agent.c_str() );
    if( rtn != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_USERAGENT failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_FOLLOWLOCATION, 1L ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_FOLLOWLOCATION failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    } 
#if 0
#if defined ( WIN32 )
    // CURLSSLOPT_NATIVE_CA is new in Curl 7.71. This tells curl to use the windows cert store. This is a beta feature
    // Otherwise we need to manually fetch the certs
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_FOLLOWLOCATION failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
#endif
#endif
    else if( callback ) {
        if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_XFERINFOFUNCTION, callback ) ) != CURLE_OK ) {
            LOG_ERROR( "CURLOPT_XFERINFOFUNCTION failed on function %x %d:%s", callback, rtn, curl_easy_strerror( rtn ) );
        }
        else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_XFERINFODATA, ctx ) ) != CURLE_OK ) {
            LOG_ERROR( "CURLOPT_XFERINFODATA failed on data %x %d:%s", ctx, rtn, curl_easy_strerror( rtn ) );
        }
    }

    return rtn;
}

int32_t PmHttp::Deinit()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_curlHandle ) {
        WLOG_DEBUG( L"curl was not initialized" );
        return 0;
    }

    curl_easy_cleanup( m_curlHandle );

    return 0;
}

int32_t PmHttp::SetToken( const std::string& token )
{
    CURLcode rtn = CURLE_OK;
    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_curlHandle ) {
        WLOG_ERROR( L"curl was not initialized" );
        return -1;
    }

    m_token = "X-Identity-Token: " + token;

    if( m_headerList ) {
        curl_slist_free_all( m_headerList );
        m_headerList = nullptr;
    }

    m_headerList = curl_slist_append( m_headerList, m_token.c_str() );

    if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_HTTPHEADER, m_headerList ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_HTTPHEADER failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }

    return rtn;
}

void PmHttp::FreeCerts()
{
    for( size_t i = 0; i < m_certList.count; ++i ) {
        X509_free( m_certList.certificates[ i ] );
        m_certList.certificates[ i ] = NULL;
    }
    free( m_certList.certificates );
    m_certList.certificates = NULL;
    m_certList.count = 0;
}

int32_t PmHttp::SetCerts( const PmHttpCertList& cert )
{
    CURLcode rtn = CURLE_OK;
    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_curlHandle ) {
        WLOG_ERROR( L"curl was not initialized" );
        return -1;
    }

    FreeCerts();

    if( cert.count > 0 ) {
        m_certList.certificates = ( X509** )calloc( cert.count, sizeof( cert.certificates[ 0 ] ) );
        for( size_t i = 0; i < cert.count; ++i ) {
            m_certList.certificates[ i ] = cert.certificates[ i ];
            ( void )X509_up_ref( m_certList.certificates[ i ] );
        }
        m_certList.count = cert.count;
    }

    if( m_certList.count ) {
        if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_SSL_CTX_FUNCTION, SslCallback ) ) != CURLE_OK ) {
            LOG_ERROR( "CURLOPT_WRITEFUNCTION failed %d:%s", rtn, curl_easy_strerror( rtn ) );
        }
        else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_SSL_CTX_DATA, &m_certList ) ) != CURLE_OK ) {
            LOG_ERROR( "CURLOPT_WRITEDATA failed %d:%s", rtn, curl_easy_strerror( rtn ) );
        }
    }

    if( rtn == CURLE_OK ) {
        if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_USE_SSL, CURLUSESSL_ALL ) ) != CURLE_OK ) {
            LOG_ERROR( "CURLOPT_WRITEFUNCTION failed %d:%s", rtn, curl_easy_strerror( rtn ) );
        }
    }

    return rtn;
}

int32_t PmHttp::HttpGet( const std::string& url, std::string& response, int32_t &httpReturn )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    CURLcode rtn = CURLE_OK;

    if( !m_curlHandle ) {
        WLOG_ERROR( L"curl was not initialized" );
        return -1;
    }

    if( url.empty() ) {
        WLOG_ERROR( L"Url was not provided" );
        return -1;
    }

    if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, WriteString ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_WRITEFUNCTION failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, &response ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_WRITEDATA failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_POST, 0 ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_POST failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_URL, url.c_str() ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_URL failed on url %s %d:%s", url.c_str(), rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_perform( m_curlHandle ) ) != CURLE_OK ) {
        LOG_ERROR( "curl_easy_perform failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_getinfo( m_curlHandle, CURLINFO_RESPONSE_CODE, &httpReturn ) ) ) {
        LOG_ERROR( "CURLINFO_RESPONSE_CODE failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else {
        LOG_DEBUG( "Curl request to %s succeeded. Http response code %d", url.c_str(), httpReturn );
    }

    return rtn;
}

int32_t PmHttp::HttpPost( const std::string& url, void* data, size_t dataSize, std::string& response, int32_t &httpReturn )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    CURLcode rtn = CURLE_OK;

    if( !m_curlHandle ) {
        WLOG_ERROR( L"curl was not initialized" );
        return -1;
    }

    if( url.empty() ) {
        WLOG_ERROR( L"Url was not provided" );
        return -1;
    }

    if( !data || !dataSize ) {
        WLOG_ERROR( L"post data was not provided" );
        return -1;
    }

    if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, WriteString ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_WRITEFUNCTION failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, &response ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_WRITEDATA failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_POST, 1 ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_POST failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_URL, url.c_str() ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_URL failed on url %s %d:%s", url.c_str(), rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_POSTFIELDS, data ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_URL failed on url %s %d:%s", url.c_str(), rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_POSTFIELDSIZE, dataSize ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_URL failed on url %s %d:%s", url.c_str(), rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_perform( m_curlHandle ) ) != CURLE_OK ) {
        LOG_ERROR( "curl_easy_perform failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_getinfo( m_curlHandle, CURLINFO_RESPONSE_CODE, &httpReturn ) ) ) {
        LOG_ERROR( "CURLINFO_RESPONSE_CODE failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else {
        LOG_DEBUG( "Curl request to %s succeeded. Http response code %d", url.c_str(), httpReturn );
    }

    return rtn;
}

int32_t PmHttp::HttpDownload( const std::string& url, const std::string& filepath, int32_t &httpReturn )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    CURLcode rtn = CURLE_OK;
    WriteFileCtx ctx = { &m_fileUtil, NULL };

    if( !m_curlHandle ) {
        WLOG_ERROR( L"curl was not initialized" );
        return -1;
    }

    if( url.empty() ) {
        WLOG_ERROR( L"Url was not provided" );
        return -1;
    }

    if( filepath.empty() ) {
        WLOG_ERROR( L"filepath was not provided" );
        return -1;
    }

    ctx.handle = m_fileUtil.PmCreateFile( filepath );
    if( ctx.handle == NULL ) {
        WLOG_ERROR( L"failed to create file %hs", filepath.c_str() );
        return -1;
    }

    if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, WriteFile ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_WRITEFUNCTION failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, &ctx ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_WRITEDATA failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_POST, 0 ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_POST failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_setopt( m_curlHandle, CURLOPT_URL, url.c_str() ) ) != CURLE_OK ) {
        LOG_ERROR( "CURLOPT_URL failed on url %s %d:%s", url.c_str(), rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_perform( m_curlHandle ) ) != CURLE_OK ) {
        LOG_ERROR( "curl_easy_perform failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else if( ( rtn = curl_easy_getinfo( m_curlHandle, CURLINFO_RESPONSE_CODE, &httpReturn ) ) ) {
        LOG_ERROR( "CURLINFO_RESPONSE_CODE failed %d:%s", rtn, curl_easy_strerror( rtn ) );
    }
    else {
        LOG_DEBUG( "Curl request to %s succeeded. Http response code %d", url.c_str(), httpReturn );
    }

    m_fileUtil.CloseFile( ctx.handle );
    ctx.handle = NULL;

    return rtn;
}
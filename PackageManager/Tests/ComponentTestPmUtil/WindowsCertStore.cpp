/**
 * @file certstore.c
 * @copyright Copyright (c) 2016 Cisco Systems, Inc.
 * @date 2016-01-28
 */

#include <Windows.h>

#include <stdint.h>

#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

static CRITICAL_SECTION _global_system_root_cert_lock;
static BOOL             _is_initialized = FALSE;

static struct {
    X509** certificates;
    size_t   count;
} _system_root_certs;

static struct {
    X509** certificates;
    size_t   count;
} _config_trusted_root_certs;

void cert_store_init( void )
{
    if( !_is_initialized ) {
        _is_initialized = TRUE;
        InitializeCriticalSection( &_global_system_root_cert_lock );
    }
}

int crypto_x509_to_pem( X509* cert,
    char** data, int* len )
{
    BIO* out = NULL;
    long pem_len = 0;
    char* pem_data = NULL;

    if( cert == NULL ||
        data == NULL ||
        len == NULL ) {
        return ( -1 );
    }

    /*
     * Output the certs to a new BIO using the PEM format
     */
    out = BIO_new( BIO_s_mem() );
    if( !out ) {
        return ( -1 );
    }

    PEM_write_bio_X509( out, cert );

    ( void )BIO_flush( out );

    /*
     * Now convert the BIO to char*
     */
    pem_len = BIO_get_mem_data( out, &pem_data );
    if( pem_len <= 0 || !pem_data ) {
        BIO_free_all( out );
        return ( -1 );
    }

    *data = ( char* )calloc( 1, pem_len + 1 );
    if( !*data ) {
        BIO_free_all( out );
        return ( -1 );
    }
    memcpy( *data, pem_data, pem_len );
    ( *data )[ pem_len ] = '\0';  //Make sure it's null termianted

    *len = ( int )pem_len;

    BIO_free_all( out );

    return ( 0 );
}

/**
 * @brief Sets up the config trusted root certs
 * @param[in] config_trusted_certs - a set of trusted certificates from config files
 * @param[in] cert_count           - a count of num of trusted certificates
 * @return 0 on success, -1 otherwise
 *
 * @attention this function assumes that the caller is holding on to the critical section
 */
int _set_config_certs( X509** config_trusted_certs, size_t cert_count )
{
    int ret = -1;

    do {
        size_t i;

        if( ( config_trusted_certs == NULL ) || ( cert_count == 0 ) ) {
            ret = 0; /* done, nothing to set */
            break;
        }

        _config_trusted_root_certs.certificates = ( X509** )calloc( cert_count,
            sizeof( *_config_trusted_root_certs.certificates ) );
        if( _config_trusted_root_certs.certificates == NULL ) {
            break;
        }

        _config_trusted_root_certs.count = 0;
        for( i = 0; i < cert_count; ++i ) {
            size_t j;
            bool found = false;

            for( j = 0; j < _system_root_certs.count; ++j ) {
                if( X509_cmp( config_trusted_certs[ i ], _system_root_certs.certificates[ j ] ) == 0 ) {
                    found = true;
                }
            }

            if( found ) {
                continue; /* certificate is already found in cert store */
            }

            _config_trusted_root_certs.certificates[ _config_trusted_root_certs.count ] =
                X509_dup( config_trusted_certs[ i ] );
            if( _config_trusted_root_certs.certificates[ _config_trusted_root_certs.count ] == NULL ) {
                continue;
            }

            _config_trusted_root_certs.count++;
        }

        ret = 0;

    } while( 0 );

    return ret;
}

int cert_store_load( X509** config_trusted_certs, size_t cert_count )
{
    const LPCWSTR pTrustedCAStoreName = L"ROOT";

    HCERTSTORE       hCertStore = NULL;
    int              ret = -1;

    EnterCriticalSection( &_global_system_root_cert_lock );

    do {
        DWORD            dwCertCount = 0;
        size_t           i = 0;
        PCCERT_CONTEXT   pCertContext = NULL;

        if( _system_root_certs.count != 0 ) {
            break;
        }

        //-------------------------------------------------------------------
        // Open a system certificate store.
        if( hCertStore = CertOpenSystemStoreW(
            ( HCRYPTPROV_LEGACY )NULL,
            pTrustedCAStoreName ) ) {
        }
        else {
            // If the store was not opened, exit to an error routine.
            break;
        }

        /* Loop through once to get a count */
        while( pCertContext = CertEnumCertificatesInStore( hCertStore,
            pCertContext ) ) {
            dwCertCount++;
        }

        if( dwCertCount == 0 ) {
            break;
        }

        _system_root_certs.certificates = ( X509** )calloc( dwCertCount,
            sizeof( *_system_root_certs.certificates ) );
        if( _system_root_certs.certificates == NULL ) {
            break;
        }

        /* Load certificates into a local cache */
        while( pCertContext = CertEnumCertificatesInStore( hCertStore,
            pCertContext ) ) {
            wchar_t pszNameString[ 256 ] = { 0 };
            X509* x509Cert = NULL;
            TCHAR* tcOutString = NULL;
            DWORD   dwSize = 0;

            if( i >= dwCertCount ) {
                break;
            }

            /* process each individual certificate */
            if( CertGetNameStringW( pCertContext,
                CERT_NAME_SIMPLE_DISPLAY_TYPE,
                0,
                NULL,
                pszNameString,
                _countof( pszNameString ) ) ) {
            }
            else {
            }

            if( !CryptBinaryToString( pCertContext->pbCertEncoded, pCertContext->cbCertEncoded,
                CRYPT_STRING_BASE64, tcOutString, &dwSize ) )
            {
                continue;
            }

            x509Cert = d2i_X509( NULL, ( const unsigned char** )&pCertContext->pbCertEncoded,
                pCertContext->cbCertEncoded );
            if( x509Cert != NULL ) {
                _system_root_certs.certificates[ i ] = x509Cert;
                _system_root_certs.count = ++i;
                /* certificate is freed on unload */
            }
            else {
            }
        }

        CertFreeCertificateContext( pCertContext );
        pCertContext = NULL;

        ret = 0;

    } while( 0 );

    CertCloseStore( hCertStore, 0 );
    hCertStore = NULL;

    if( ( _system_root_certs.count == 0 ) && ( _system_root_certs.certificates != NULL ) ) {
        /*
         *  We allocated memory to hold the certificates but we were unable to
         *  convert the certificates into X509 format (i.e. invalid certificate)
         */
        free( _system_root_certs.certificates );
        _system_root_certs.certificates = NULL;

        /* no certs were loaded */
        ret = -1;
    }

    /* Now apply the trusted certs from config */
    if( ret == 0 ) {
        if( _set_config_certs( config_trusted_certs, cert_count ) != 0 ) {
            /* proceed as if we succeeded using only certificates from the system */
        }
    }

    LeaveCriticalSection( &_global_system_root_cert_lock );

    return ret;
}

int cert_store_unload()
{
    size_t i;

    if( !_is_initialized ) return 0;

    EnterCriticalSection( &_global_system_root_cert_lock );

    if( _system_root_certs.certificates != NULL ) {
        for( i = 0; i < _system_root_certs.count; ++i ) {
            X509_free( _system_root_certs.certificates[ i ] );
            _system_root_certs.certificates[ i ] = NULL;
        }

        free( _system_root_certs.certificates );
    }

    _system_root_certs.certificates = NULL;
    _system_root_certs.count = 0U;

    if( _config_trusted_root_certs.certificates != NULL ) {
        for( i = 0; i < _config_trusted_root_certs.count; ++i ) {
            X509_free( _config_trusted_root_certs.certificates[ i ] );
            _config_trusted_root_certs.certificates[ i ] = NULL;
        }
        free( _config_trusted_root_certs.certificates );
    }

    _config_trusted_root_certs.certificates = NULL;
    _config_trusted_root_certs.count = 0U;

    LeaveCriticalSection( &_global_system_root_cert_lock );

    return 0;
}

int cert_store_config_trusted_cert_update( X509** config_trusted_certs,
    size_t cert_count )
{
    int    ret = -1;
    size_t i;

    EnterCriticalSection( &_global_system_root_cert_lock );

    if( _config_trusted_root_certs.certificates != NULL ) {
        for( i = 0; i < _config_trusted_root_certs.count; ++i ) {
            X509_free( _config_trusted_root_certs.certificates[ i ] );
            _config_trusted_root_certs.certificates[ i ] = NULL;
        }
        free( _config_trusted_root_certs.certificates );
    }

    _config_trusted_root_certs.certificates = NULL;
    _config_trusted_root_certs.count = 0U;

    ret = _set_config_certs( config_trusted_certs, cert_count );

    LeaveCriticalSection( &_global_system_root_cert_lock );

    return ret;
}

void _add_to_x509_store( X509_STORE* store, X509** certs, size_t count )
{
    size_t i;
    for( i = 0; i < count; ++i ) {
        if( X509_STORE_add_cert( store, certs[ i ] ) != 1 ) {
            unsigned long err = ERR_get_error();
            if( X509_R_CERT_ALREADY_IN_HASH_TABLE == ERR_GET_REASON( err ) ) {
            }
            else {
            }
        }
        else {
        }
    }
}

void cert_add_from_keystore( X509_STORE* store, X509* additional_ca )
{
    EnterCriticalSection( &_global_system_root_cert_lock );

    do {
        if( ( _system_root_certs.certificates == NULL ) || ( _system_root_certs.count == 0U ) ) {
            break;
        }

        if( store == NULL ) {
            break;
        }

        /* add from OS cert store */
        _add_to_x509_store( store, _system_root_certs.certificates,
            _system_root_certs.count );

        /* add from config trusted certs */
        _add_to_x509_store( store, _config_trusted_root_certs.certificates,
            _config_trusted_root_certs.count );

        /* Adding the additional CA cert to the trustchain */
        if( ( additional_ca != NULL ) && ( X509_STORE_add_cert( store, additional_ca ) != 1 ) ) {
            unsigned long err = ERR_get_error();
            if( X509_R_CERT_ALREADY_IN_HASH_TABLE == ERR_GET_REASON( err ) ) {

            }
            else {

            }
        }
    } while( 0 );

    LeaveCriticalSection( &_global_system_root_cert_lock );
}

typedef enum {
    PEM_APPEND_OK,
    PEM_APPEND_NOMEM,
    PEM_APPEND_NULLARG,
    PEM_APPEND_ERROR_UNKNOWN,
} pem_append_error_t;

static pem_append_error_t
_fm_x509_to_pem_append( X509* ca_cert, char** cert_data, int* total_buf_len,
    size_t* remaining_buf_len )
{
    char* pem_data = NULL;
    int pem_data_len = 0;
    pem_append_error_t ret = PEM_APPEND_ERROR_UNKNOWN;
    int current_len = 0;

    if( ca_cert == NULL || total_buf_len == NULL ||
        remaining_buf_len == NULL || *cert_data == NULL ) {
        ret = PEM_APPEND_NULLARG;
        goto done;
    }

    current_len = *total_buf_len;

    if( crypto_x509_to_pem( ca_cert, &pem_data, &pem_data_len ) != 0 ) {
        goto done;
    }

    if( pem_data_len > ( int )*remaining_buf_len ) {
        char* tmp = ( char* )realloc( *cert_data, current_len + pem_data_len + 1 );
        if( tmp == NULL ) {

            free( *cert_data );
            *cert_data = NULL;
            *total_buf_len = 0;
            ret = PEM_APPEND_NOMEM;

            goto done;
        }
        *cert_data = tmp;
        *remaining_buf_len = 0;
    }
    else {
        *remaining_buf_len -= pem_data_len;
    }

    memcpy( &( ( *cert_data )[ current_len ] ), pem_data, pem_data_len );
    *total_buf_len = current_len + pem_data_len;
    ( *cert_data )[ *total_buf_len ] = '\0';

    ret = PEM_APPEND_OK;

done:
    free( pem_data );
    pem_data = NULL;
    return ret;
}

int cert_get_pem_certs_from_keystore( char** cert_data, int* cert_data_len, X509* additional_ca_cert )
{
    const uint32_t STARTING_RAW_PEM_LENGTH = 350 * 1024;

    uint32_t i;
    int      ret = -1;

    EnterCriticalSection( &_global_system_root_cert_lock );

    do {
        size_t remaining_buf_len = 0;
        bool   add_additional_ca_cert = true;

        if( ( cert_data == NULL ) || ( cert_data_len == NULL ) ) {
            break;
        }

        *cert_data = ( char* )calloc( 1, STARTING_RAW_PEM_LENGTH );
        if( *cert_data == NULL ) {
            break;
        }
        *cert_data_len = 0;
        remaining_buf_len = STARTING_RAW_PEM_LENGTH;

        if( ( _system_root_certs.certificates == NULL ) || ( _system_root_certs.count == 0U ) ) {
            ret = 0; /* not having certificates loaded is not an error, not a requirement */
            break;
        }

        for( i = 0; i < _system_root_certs.count; ++i ) {
            int ret = _fm_x509_to_pem_append( _system_root_certs.certificates[ i ],
                cert_data,
                cert_data_len,
                &remaining_buf_len );

            if( ret == PEM_APPEND_NOMEM ) {
                break;
            }
            else if( ret == PEM_APPEND_OK ) {
                /*
                ** Two certs by the same name can cause conflicts.  Trust the one in the
                ** OS certificate/key store if the additional CA name matches that of
                ** one in the store.
                */
#if OPENSSL_VERSION_NUMBER < 0x10100000L
                if( additional_ca_cert && additional_ca_cert->cert_info &&
                    ( strcmp( _system_root_certs.certificates[ i ]->name, additional_ca_cert->name ) == 0 ) ) {
#else
                if( additional_ca_cert && X509_get_subject_name( additional_ca_cert ) &&
                    ( X509_name_cmp( X509_get_subject_name( _system_root_certs.certificates[ i ] ), X509_get_subject_name( additional_ca_cert ) ) == 0 ) ) {
#endif
                    add_additional_ca_cert = false;
                }
                }
            }

        for( i = 0; i < _config_trusted_root_certs.count; ++i ) {
            int ret = _fm_x509_to_pem_append( _config_trusted_root_certs.certificates[ i ],
                cert_data,
                cert_data_len,
                &remaining_buf_len );

            if( ret == PEM_APPEND_NOMEM ) {
                break;
            }
            else if( ret == PEM_APPEND_OK ) {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
                if( additional_ca_cert && additional_ca_cert->cert_info &&
                    ( strcmp( _system_root_certs.certificates[ i ]->name, additional_ca_cert->name ) == 0 ) ) {
#else
                if( additional_ca_cert && X509_get_subject_name( additional_ca_cert ) &&
                    ( X509_name_cmp( X509_get_subject_name( _system_root_certs.certificates[ i ] ), X509_get_subject_name( additional_ca_cert ) ) == 0 ) ) {
#endif
                    add_additional_ca_cert = false;
                }
                }
            }

        /* End with the additional CA certificate if provided */
        if( additional_ca_cert && add_additional_ca_cert && *cert_data ) {
            /* Return an error only if we were unable to allocate memory */
            if( _fm_x509_to_pem_append( additional_ca_cert,
                cert_data,
                cert_data_len,
                &remaining_buf_len ) == PEM_APPEND_NOMEM ) {
                break;
            }
        }

        if( *cert_data != NULL ) {
            ret = 0;
        }

        } while( 0 );

        LeaveCriticalSection( &_global_system_root_cert_lock );

        return ret;
        }

int cert_store_retrieve_certs( X509 * **trusted_certificates, size_t * cert_count )
{
    int ret = -1;
    EnterCriticalSection( &_global_system_root_cert_lock );

    do
    {
        *cert_count = 0;
        size_t total_count = _system_root_certs.count + _config_trusted_root_certs.count;
        *trusted_certificates = ( X509** )calloc( total_count, sizeof( *trusted_certificates ) );
        if( ( *trusted_certificates ) == NULL ) {
            break;
        }

        for( size_t i = 0; i < _system_root_certs.count; i++ ) {
            ( *trusted_certificates )[ *cert_count ] =
                X509_dup( _system_root_certs.certificates[ i ] );
            ( *cert_count )++;
        }

        for( size_t i = 0; i < _config_trusted_root_certs.count; i++ ) {
            ( *trusted_certificates )[ *cert_count ] =
                X509_dup( _config_trusted_root_certs.certificates[ i ] );
            ( *cert_count )++;
        }

        ret = 0;

    } while( 0 );

    LeaveCriticalSection( &_global_system_root_cert_lock );
    return ret;
}

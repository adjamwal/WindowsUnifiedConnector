#include "pch.h"
#include "WinCertLoader.h"
#include "IUcLogger.h"
#include <wincrypt.h>
#include <stdint.h>

#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

WinCertLoader::WinCertLoader() :
    m_certcount( 0 )
    , m_certificates( nullptr )
{

}

WinCertLoader::~WinCertLoader()
{

}

int WinCertLoader::LoadSystemCerts()
{
    const LPCWSTR pTrustedCAStoreName = L"ROOT";

    HCERTSTORE       hCertStore = NULL;
    int              ret = -1;

    std::lock_guard<std::mutex> lock( m_mutex );

    do {
        DWORD            dwCertCount = 0;
        size_t           i = 0;
        PCCERT_CONTEXT   pCertContext = NULL;

        if( m_certcount != 0 ) {
            WLOG_WARNING( L"System certs already loaded" );
            break;
        }

        //-------------------------------------------------------------------
        // Open a system certificate store.
        if( hCertStore = CertOpenSystemStore(
            ( HCRYPTPROV_LEGACY )NULL,
            pTrustedCAStoreName ) ) {
        }
        else {
            WLOG_ERROR( L"CertOpenSystemStoreW failed" );
            break;
        }

        /* Loop through once to get a count */
        while( pCertContext = CertEnumCertificatesInStore( hCertStore,
            pCertContext ) ) {
            dwCertCount++;
        }

        if( dwCertCount == 0 ) {
            WLOG_ERROR( L"No certs found" );
            break;
        }

        m_certificates = ( X509** )calloc( dwCertCount,
            sizeof( *m_certificates ) );
        if( m_certificates == NULL ) {
            WLOG_ERROR( L"Failed to allocate certificates" );
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
                m_certificates[ i ] = x509Cert;
                m_certcount = ++i;
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

    if( ( m_certcount == 0 ) && ( m_certificates != NULL ) ) {
        /*
         *  We allocated memory to hold the certificates but we were unable to
         *  convert the certificates into X509 format (i.e. invalid certificate)
         */
        free( m_certificates );
        m_certificates = NULL;

        /* no certs were loaded */
        WLOG_ERROR( L"Failed to covert certficates" );
        ret = -1;
    }

    return ret;
}

void WinCertLoader::FreeCerts( X509** trusted_certificates, size_t cert_count )
{
    size_t i;
    if( m_certificates != NULL ) {
        for( i = 0; i < m_certcount; ++i ) {
            X509_free( m_certificates[ i ] );
            m_certificates[ i ] = NULL;
        }

        free( m_certificates );
        m_certificates = NULL;
    }
}

int WinCertLoader::UnloadSystemCerts()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    FreeCerts( m_certificates, m_certcount );

    m_certificates = NULL;
    m_certcount = 0U;

    return 0;
}


int WinCertLoader::GetSystemCerts( X509*** trusted_certificates, size_t& cert_count )
{
    int ret = -1;
    std::lock_guard<std::mutex> lock( m_mutex );

    cert_count = 0;
    *trusted_certificates = ( X509** )calloc( m_certcount, sizeof( *trusted_certificates ) );

    if( ( *trusted_certificates ) != NULL ) {
        for( size_t i = 0; i < m_certcount; i++ ) {
            ( *trusted_certificates )[ cert_count ] =
                X509_dup( m_certificates[ i ] );
            ( cert_count )++;
        }

        ret = 0;

    }

    return ret;
}

int WinCertLoader::FreeSystemCerts( X509** trusted_certificates, size_t cert_count )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    FreeCerts( trusted_certificates, cert_count );

    return 0;
}
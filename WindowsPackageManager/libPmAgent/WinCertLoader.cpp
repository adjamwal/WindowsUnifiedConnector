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
    const LPCWSTR pTrustedCAStoreName = L"ROOT";  // Root Certificate Authority store
    const LPCWSTR pIntermediateCAStoreName = L"CA";  // Intermediate Certificate Authority store
  
    int              ret = -1;
    DWORD            dwCertCount = 0;
    std::lock_guard<std::mutex> lock( m_mutex );
    
    if( m_certcount != 0 ) {
        WLOG_WARNING( L"System certs already loaded" );
        return ret;
    }
    std::vector<X509*> certList;
    m_certcount = 0;

    // Process certificates from the ROOT store
    if (!LoadCertificatesFromStore(pTrustedCAStoreName, certList)) {
        WLOG_ERROR(L"Failed to process certificates from ROOT store");
        return ret;
    }

    // Process certificates from the CA store
    if (!LoadCertificatesFromStore(pIntermediateCAStoreName, certList)) {
        WLOG_ERROR(L"Failed to process certificates from CA store");
        return ret;
    }

    if( certList.empty() ) {
        WLOG_ERROR( L"No certs found" );
        return ret;
    }
    dwCertCount = static_cast<DWORD>(certList.size());
    m_certificates = ( X509** )calloc( dwCertCount,
        sizeof( *m_certificates ) );
    if( m_certificates == NULL ) {
        WLOG_ERROR( L"Failed to allocate certificates" );
        return ret;
    }
    
    for (DWORD i = 0; i < dwCertCount; ++i) {
        m_certificates[i] = certList[i];
        m_certcount++;
    }
    ret = 0;
    return ret;
}

/**
 * Opens a certificate store, enumerates certificates and converts them to X509 format
 */
bool WinCertLoader::LoadCertificatesFromStore(const LPCWSTR storeName, std::vector<X509*>& certList)
{
    HCERTSTORE       hCertStore = NULL;
    PCCERT_CONTEXT   pCertContext = NULL;

    if( hCertStore = CertOpenSystemStore(( HCRYPTPROV_LEGACY )NULL,storeName ) ) {
    }
    else {
        WLOG_ERROR( L"CertOpenSystemStoreW failed" );
        return false;
    }
    while( pCertContext = CertEnumCertificatesInStore( hCertStore, pCertContext ) ) 
    {
        wchar_t pszNameString[ 256 ] = { 0 };
        X509* x509Cert = NULL;
        TCHAR* tcOutString = NULL;
        DWORD   dwSize = 0;

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
            certList.emplace_back(x509Cert);
        }
        else {
        }
    }
    if(NULL != pCertContext)
    {
        CertFreeCertificateContext(pCertContext);
        pCertContext = NULL;
    }
    if(NULL != hCertStore)
    {
        CertCloseStore(hCertStore, 0);
        hCertStore = NULL;
    }  
    return true;
}

void WinCertLoader::FreeCerts( X509** trusted_certificates, size_t cert_count )
{
    size_t i;
    if( trusted_certificates != NULL ) {
        for( i = 0; i < cert_count; ++i ) {
            X509_free( trusted_certificates[ i ] );
            trusted_certificates[ i ] = NULL;
        }

        free( trusted_certificates );
        trusted_certificates = NULL;
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
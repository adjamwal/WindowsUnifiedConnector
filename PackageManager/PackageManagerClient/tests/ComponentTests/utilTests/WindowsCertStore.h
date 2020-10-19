#pragma once

#if defined( WIN32 )
#include <openssl/x509.h>

/**
    * @brief initialize the cert store
    *
    * @note Call before cert_store_load()
    */
void cert_store_init( void );

/**
    * @brief load the Windows cert store certificates
    * @param[in] config_trusted_certs - a set of trusted certificates from config files
    * @param[in] cert_count           - a count of num of trusted certificates
    * @return 0 on success, -1 otherwise
    *
    * @note Should be called after OpenSSL is initialized
    *       *Uses OpenSSL routines*
    */
int cert_store_load( X509** config_trusted_certs, size_t cert_count );

/**
    * @brief unload the windows cert store certificates
    * @return 0 on success, -1 otherwise
    */
int cert_store_unload( void );

/**
    * @brief Update the trusted certs loaded from config
    *        frees any previously loaded config certs and adds the newly
    *        given certs
    * @param[in] config_trusted_certs - a set of trusted certificates from config files
    * @param[in] cert_count           - a count of num of trusted certificates
    * @return 0 on success, -1 otherwise
    */
int cert_store_config_trusted_cert_update( X509** config_trusted_certs,
    size_t cert_count );

/**
    * @brief Used as a call back by Curl to load
    *        certificates to it's OpenSSL X509 store
    *        context
    * @param[in] store - X509 store context
    * @param[in] additional_ca - additional CA root to add to store
    */
void cert_add_from_keystore( X509_STORE* store, X509* additional_ca );

/**
    * @brief Converts certificates from cert store to a string of PEM certs
    * @param[out] raw_data     - The PEM formatted certs
    * @param[out] raw_data_len - Size of the PEM cert buffer
    * @param[in]  additional_ca - additional CA root to convert to PEM
    * @return 0 on success, -1 otherwise
    */
int cert_get_pem_certs_from_keystore( char** raw_data, int* raw_data_len,
    X509* additional_ca_cert );

int cert_store_retrieve_certs( X509*** trusted_certificates, size_t* cert_count );

#endif /* WIN32 */
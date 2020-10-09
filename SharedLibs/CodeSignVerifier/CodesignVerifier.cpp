/**************************************************************************
 * Copyright (c) 2020, Cisco Systems, All Rights Reserved
 ***************************************************************************
 *
 * @file : CodesignVerifierWin.cpp
 *
 ***************************************************************************
 *
 * Implements file signature verification
 *
 ***************************************************************************/
#include "pch.h"

#include "CodesignVerifier.h"
#include <Windows.h>
#include <Softpub.h>
#include <Mscat.h>
#include "IUcLogger.h"

#define KILLDATE ((unsigned long long)1337659200)

const DWORD ENCODING = (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING);

void convertFileTime1601To1970(const FILETIME& rFileTime, uint64_t& pTime_t)
{
    // A FILETIME is the number of 100-nanosecond intervals since January 1, 1601.
    // A uint64_t is the number of 1-second intervals since January 1, 1970.
    const uint64_t FT_TO_SECS = 10000000ULL;
    const uint64_t FT_TO_TIMET_DIFF_SECS = 11644473600ULL;
    ULARGE_INTEGER ull;
    ull.LowPart = rFileTime.dwLowDateTime;
    ull.HighPart = rFileTime.dwHighDateTime;
    pTime_t = (uint64_t)(ull.QuadPart / FT_TO_SECS - FT_TO_TIMET_DIFF_SECS);
}

CodesignStatus verify_by_file( const std::wstring& rtstrPath, const std::wstring& rtstrSigner, SigType sig_type, uint64_t killdate)
{
    CodesignStatus retStatus = CodesignStatus::CODE_SIGNER_ERROR;
    uint64_t timeStamp = 0;
    LPTSTR tszCommonName = NULL;
    std::wstring tstrSignerName;
    WINTRUST_DATA wintrustdata;
    WINTRUST_FILE_INFO fileinfo;
    FILETIME sigTimestamp;
    DWORD dwStrLen = 0;

    CRYPT_PROVIDER_DATA *pProvData = NULL;
    CRYPT_PROVIDER_SGNR *pProvSigner = NULL;
    CRYPT_PROVIDER_CERT *pProvCert   = NULL;

    ZeroMemory(&fileinfo, sizeof(fileinfo));
    ZeroMemory(&wintrustdata, sizeof(wintrustdata));
    ZeroMemory(&sigTimestamp, sizeof(FILETIME));

    //populate entries for struct WINTRUST_FILE_INFO
    fileinfo.cbStruct = sizeof(fileinfo);
    fileinfo.pcwszFilePath = rtstrPath.c_str();
    fileinfo.pgKnownSubject = NULL;
    fileinfo.hFile = NULL;

    //populate entries for struct WINTRUST_DATA
    wintrustdata.cbStruct = sizeof(wintrustdata);
    wintrustdata.pPolicyCallbackData = NULL;
    wintrustdata.pSIPClientData = NULL;
    wintrustdata.fdwRevocationChecks = WTD_REVOKE_NONE;
    wintrustdata.dwStateAction = WTD_STATEACTION_VERIFY;
    wintrustdata.hWVTStateData = NULL;
    wintrustdata.pwszURLReference = NULL;
    wintrustdata.dwUIChoice = WTD_UI_NONE;
    wintrustdata.dwProvFlags = WTD_REVOCATION_CHECK_NONE;
    wintrustdata.dwUnionChoice = WTD_CHOICE_FILE;
    wintrustdata.pFile = &fileinfo;

    GUID guidPublishedSoftware = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    HRESULT hr = WinVerifyTrustEx((HWND)INVALID_HANDLE_VALUE, &guidPublishedSoftware, &wintrustdata);
    if (ERROR_SUCCESS != hr)
    {
        WLOG_ERROR( L"unable to verify trust for [%s]", rtstrPath.c_str() );
        goto safe_exit;
    }

    pProvData = WTHelperProvDataFromStateData(wintrustdata.hWVTStateData);
    if (NULL == pProvData)
    {
        WLOG_ERROR( L"unable to get Data using WTHelperProvDataFromStateData for [%s]", rtstrPath.c_str() );
        goto safe_exit;
    }

    pProvSigner = WTHelperGetProvSignerFromChain(pProvData, 0, FALSE, 0);
    if (NULL == pProvSigner)
    {
        WLOG_ERROR( L"unable to get signer from chain for [%s]", rtstrPath.c_str() );
        goto safe_exit;
    }

    pProvCert = WTHelperGetProvCertFromChain(pProvSigner, 0);
    if (NULL == pProvCert)
    {
        WLOG_ERROR( L"unable to get signer from chain for [%ls]", rtstrPath.c_str() );
        goto safe_exit;
    }

    // get the length of the desired string (includes terminating NULL)
    dwStrLen = CertGetNameString(pProvCert->pCert,
                                        CERT_NAME_ATTR_TYPE,
                                        0,
                                        ( void* )szOID_COMMON_NAME,
                                        NULL,
                                        0);
    // get the string, if it exists
    if (dwStrLen > 1)
    {
        tszCommonName = reinterpret_cast<LPTSTR>(::LocalAlloc(0, dwStrLen * sizeof(TCHAR)));
        CertGetNameString(pProvCert->pCert, CERT_NAME_ATTR_TYPE, 0, ( void* )szOID_COMMON_NAME, tszCommonName, dwStrLen);
        tstrSignerName.assign(tszCommonName);
        ::LocalFree(tszCommonName);
    }

    // only check the CN if one was supplied by the caller.
    if (!tstrSignerName.empty())
    {
        // bail out if the signer does not match expected signer
        if (rtstrSigner == tstrSignerName)
        {
            // found a valid match
            WLOG_DEBUG( L"found a valid match for [%s], [%s]", rtstrPath.c_str(), tstrSignerName.c_str() );
        }
        else
        {
            WLOG_ERROR( L"Cert signer name didn't match for [%ls], [%ls]", rtstrPath.c_str(), tstrSignerName.c_str() );
            retStatus = CodesignStatus::CODE_SIGNER_MISMATCH;
            goto safe_exit;
        }
    }

    if (pProvSigner->csCounterSigners > 0)
    {
        FileTimeToLocalFileTime(&pProvSigner->pasCounterSigners[0].sftVerifyAsOf, &sigTimestamp);
        convertFileTime1601To1970(sigTimestamp, timeStamp);
        WLOG_DEBUG( L"path and timestamp is [%ls] - [%llu]", rtstrPath.c_str(), timeStamp );
    }
    else
    {
        LOG_WARNING( "Failed to get the timestamp" );
    }

    if ( SIGNER_CISCO == rtstrSigner)
    {
        /* verify timestamp against killdate */
        if ( timeStamp < killdate )
        {
            WLOG_ERROR( L"timestamp expired for file: [%s].", rtstrPath.c_str() );
            retStatus = CodesignStatus::CODE_SIGNER_EXPIRED;
            goto safe_exit;
        }
    }

    retStatus = CodesignStatus::CODE_SIGNER_SUCCESS;

safe_exit:
    // Any hWVTStateData must be released by a call with close.
    wintrustdata.dwStateAction = WTD_STATEACTION_CLOSE;
    hr = WinVerifyTrustEx((HWND)INVALID_HANDLE_VALUE, &guidPublishedSoftware, &wintrustdata);
    if (ERROR_SUCCESS != hr)
    {
        WLOG_ERROR( L"failed to release trust for [%s]", rtstrPath.c_str() );
    }

    return retStatus;
}

CodesignStatus verify_by_catalog( const std::wstring& rtstrPath, const std::wstring& rtstrSigner, SigType sig_type )
{
    /* only trust MS files for catalog verification. */
    if ( SIGNER_MICROSOFT != rtstrSigner)
    {
        return CodesignStatus::CODE_SIGNER_ERROR;
    }

    CodesignStatus status = CodesignStatus::CODE_SIGNER_ERROR;

    HCATADMIN cat_admin_ctx = NULL;
    HANDLE file_handle = INVALID_HANDLE_VALUE;
    DWORD file_hash_size = 0;
    BYTE* file_hash = NULL;
    TCHAR* member_tag = NULL;
    HCATINFO cat_ctx = NULL;
    CATALOG_INFO cat_info;
    WINTRUST_DATA wintrust_data;
    WINTRUST_CATALOG_INFO wintrust_cat_info;
    GUID ActionGuid = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    unsigned int i = 0;

    WLOG_DEBUG( L"checking signature by catalog [%s]", rtstrPath.c_str() );

    if( FALSE == CryptCATAdminAcquireContext(&cat_admin_ctx, NULL, 0) || NULL == cat_admin_ctx )
    {
        goto safe_exit;
    }

    file_handle = CreateFile( rtstrPath.c_str() , GENERIC_READ, 7, NULL, OPEN_EXISTING, 0, NULL);
    if( INVALID_HANDLE_VALUE == file_handle )
    {
        goto safe_exit;
    }

    if( FALSE == CryptCATAdminCalcHashFromFileHandle( file_handle, &file_hash_size, NULL, 0) ||
        0 == file_hash_size )
    {
        goto safe_exit;
    }

    file_hash = ( BYTE* ) calloc( file_hash_size, 1 );
    if( NULL == file_hash )
    {
        goto safe_exit;
    }

    if( FALSE == CryptCATAdminCalcHashFromFileHandle( file_handle, &file_hash_size, file_hash, 0 ) ||
        0 == file_hash_size )
    {
        goto safe_exit;
    }

    member_tag = (TCHAR*) calloc( ( file_hash_size + 1 ) * 2, sizeof( TCHAR) );
    if( NULL == member_tag )
    {
        goto safe_exit;
    }
    for( i = 0; i < file_hash_size; i++ )
    {
        swprintf_s( &member_tag[i * 2], sizeof(TCHAR)*2, _T("%02X"), file_hash[i] );
    }

    cat_ctx = CryptCATAdminEnumCatalogFromHash( cat_admin_ctx, file_hash, file_hash_size, 0, NULL);
    if( NULL == cat_ctx )
    {
        goto safe_exit;
    }

    ZeroMemory( (void*) &cat_info, sizeof( cat_info ) );
    if( FALSE == CryptCATCatalogInfoFromContext( cat_ctx, &cat_info, 0) )
    {
        goto safe_exit;
    }

    ZeroMemory( (void*) &wintrust_data, sizeof( wintrust_data ) );
    wintrust_data.cbStruct = sizeof(wintrust_data);
    wintrust_data.pPolicyCallbackData = NULL;
    wintrust_data.pSIPClientData = NULL;
    wintrust_data.dwUIChoice = WTD_UI_NONE;
    wintrust_data.fdwRevocationChecks = WTD_REVOKE_NONE;
    wintrust_data.dwUnionChoice = WTD_CHOICE_CATALOG;
    wintrust_data.pCatalog = &wintrust_cat_info;
    wintrust_data.dwStateAction = WTD_STATEACTION_IGNORE;//WTD_STATEACTION_VERIFY;
    wintrust_data.hWVTStateData = NULL;
    wintrust_data.pwszURLReference = NULL;
    wintrust_data.dwProvFlags = WTD_REVOCATION_CHECK_NONE;//0;
    wintrust_data.dwUIContext = WTD_UICONTEXT_EXECUTE;

    ZeroMemory( (void*) &wintrust_cat_info, sizeof( wintrust_cat_info ) );
    wintrust_cat_info.cbStruct = sizeof( wintrust_cat_info );
    wintrust_cat_info.dwCatalogVersion = 0;
    wintrust_cat_info.pcwszCatalogFilePath = cat_info.wszCatalogFile;
    wintrust_cat_info.pcwszMemberTag = member_tag ;
    wintrust_cat_info.pcwszMemberFilePath = rtstrPath.c_str();
    wintrust_cat_info.hMemberFile = NULL;

    if( 0 != WinVerifyTrust( 0, &ActionGuid, &wintrust_data ) )
    {
        WLOG_ERROR( L"unable to verify trust for [%s]", rtstrPath.c_str() );
        goto safe_exit;
    }

    status = CodesignStatus::CODE_SIGNER_SUCCESS;

safe_exit:

    if( NULL != cat_ctx )
    {
        CryptCATAdminReleaseCatalogContext( cat_admin_ctx, cat_ctx, 0);
        cat_ctx = NULL;
    }

    if( NULL != member_tag )
    {
        free( member_tag );
        member_tag = NULL;
    }

    if( NULL != file_hash )
    {
        free( file_hash );
        file_hash = NULL;
    }

    if( INVALID_HANDLE_VALUE != file_handle )
    {
        CloseHandle( file_handle );
        file_handle = INVALID_HANDLE_VALUE;
    }

    if( NULL != cat_admin_ctx )
    {
        CryptCATAdminReleaseContext( cat_admin_ctx, 0 );
        cat_admin_ctx = NULL;
    }

    return status;
}


CodesignStatus CodesignVerifier::VerifyWithKilldate( const std::wstring& rtstrPath, const std::wstring& rtstrSigner, SigType sig_type, uint64_t killdate )
{
    WLOG_DEBUG( L"verifying file signature: file = [%s], signer = [%s], type = [%d]", rtstrPath.c_str(), rtstrSigner.c_str(), sig_type );
    if ( CodesignStatus::CODE_SIGNER_SUCCESS == verify_by_file( rtstrPath, rtstrSigner, sig_type, killdate))
    {
        LOG_DEBUG("file signature verified by file." );
    }

    else if ( CodesignStatus::CODE_SIGNER_SUCCESS == verify_by_catalog( rtstrPath, rtstrSigner, sig_type))
    {
        LOG_DEBUG( "file signature verified by catalog." );
    }

    else
    {
        WLOG_ERROR( L"unable to verify file signature: [%s]", rtstrPath.c_str() );
        return CodesignStatus::CODE_SIGNER_VERIFICATION_FAILED;
    }

    return CodesignStatus::CODE_SIGNER_SUCCESS;
}

CodesignStatus CodesignVerifier::Verify( const std::wstring& rtstrPath, const std::wstring& rtstrSigner, SigType sig_type )
{
    if( (rtstrPath.empty()) || (rtstrSigner.empty()) || (SigType::SIGTYPE_NATIVE != sig_type))
    {
        WLOG_ERROR( L"invalid parameters [%s] : [%s]", rtstrPath.c_str(), rtstrSigner.c_str() );
        return CodesignStatus::CODE_SIGNER_INVALID;
    }
    return VerifyWithKilldate( rtstrPath, rtstrSigner, sig_type, KILLDATE );
}
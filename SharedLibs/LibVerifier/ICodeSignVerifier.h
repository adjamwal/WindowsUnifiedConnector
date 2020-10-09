#pragma once

#include <string>
#include <tchar.h>

#define SIGNER_CISCO_CN     _T("Cisco Systems, Inc.")
#define SIGNER_MICROSOFT    _T("Microsoft Corporation")
#define SIGNER_MOZILLA      _T("Mozilla Corporation")

#ifdef _WIN32
#define SIGNER_CISCO        SIGNER_CISCO_CN
#endif

enum class SigType
{
    SIGTYPE_AUTO = 0,
    SIGTYPE_NATIVE,
    SIGTYPE_CISCO
};

#ifdef _WIN32
#define SIGTYPE_DEFAULT     SigType::SIGTYPE_NATIVE
#endif

enum class CodesignStatus
{
    CODE_SIGNER_SUCCESS = 0,
    CODE_SIGNER_ERROR,
    CODE_SIGNER_INVALID,
    CODE_SIGNER_EXPIRED,
    CODE_SIGNER_MISMATCH,
    CODE_SIGNER_VERIFICATION_FAILED
};

class ICodesignVerifier
{
public:
    virtual CodesignStatus Verify( const std::wstring& rtstrPath, const std::wstring& rtstrSigner, SigType sig_type ) = 0;
    virtual CodesignStatus VerifyWithKilldate( const std::wstring& rtstrPath, const std::wstring& rtstrSigner, SigType sig_type, uint64_t killdate ) = 0;
};
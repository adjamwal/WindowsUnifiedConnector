/**************************************************************************
 * Copyright (c) 2020, Cisco Systems, All Rights Reserved
 ***************************************************************************
 *
 * @file : CodesignVerifier.h
 *
 ***************************************************************************
 *
 * Defines file signature verification
 *
 ***************************************************************************/

#pragma once

#include "PmTypes.h"
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

class CodesignVerifier
{
    public:
        PM_STATUS Verify( const std::wstring& rtstrPath, const std::wstring& rtstrSigner, SigType sig_type );
        PM_STATUS VerifyWithKilldate( const std::wstring& rtstrPath, const std::wstring& rtstrSigner, SigType sig_type, uint64_t killdate );
};

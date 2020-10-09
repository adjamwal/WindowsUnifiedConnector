
#pragma once

#include "ICodeSignVerifier.h"

class CodesignVerifier : public ICodesignVerifier
{
public:
    CodesignStatus Verify( 
        const std::wstring& rtstrPath, 
        const std::wstring& rtstrSigner, 
        SigType sig_type ) override;
    CodesignStatus VerifyWithKilldate( 
        const std::wstring& rtstrPath, 
        const std::wstring& rtstrSigner, 
        SigType sig_type, 
        uint64_t killdate ) override;
};

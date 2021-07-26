#pragma once

#include "pch.h"
#include "ICodesignVerifier.h"

class MockCodesignVerifier : public ICodesignVerifier
{
public:
    MockCodesignVerifier();
    ~MockCodesignVerifier();

    MOCK_METHOD( CodesignStatus, Verify, (const std::wstring& rtstrPath, const std::wstring& rtstrSigner, SigType sig_type) );
    void MakeVerifyReturn( CodesignStatus );
    void ExpectVerifyIsNotCalled();

    MOCK_METHOD( CodesignStatus, VerifyWithKilldate, (const std::wstring& rtstrPath, const std::wstring& rtstrSigner, SigType sig_type, uint64_t killdate) );
    void MakeVerifyWithKilldateReturn( CodesignStatus );
    void ExpectVerifyWithKilldateIsNotCalled();
};
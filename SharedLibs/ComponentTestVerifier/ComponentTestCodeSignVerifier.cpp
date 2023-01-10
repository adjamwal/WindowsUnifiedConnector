#include "pch.h"
#include "CodesignVerifier.h"
#include <memory>

class ComponentTestCodeSignVerifier: public ::testing::Test
{
protected:
    void SetUp()
    {
        m_patient.reset( new CodesignVerifier() );
    }

    void TearDown()
    {
        m_patient.reset();
    }

    std::unique_ptr<CodesignVerifier> m_patient;
};

TEST_F( ComponentTestCodeSignVerifier, VerifyCisco )
{
    EXPECT_EQ( m_patient->Verify(
        L"..\\..\\SharedLibs\\ComponentTestVerifier\\CodeSignTestCisco",
        SIGNER_CISCO_CN,
        SIGTYPE_DEFAULT ), CodesignStatus::CODE_SIGNER_SUCCESS );
}

TEST_F( ComponentTestCodeSignVerifier, VerifyCiscoList )
{
    EXPECT_EQ( m_patient->Verify(
        L"..\\..\\SharedLibs\\ComponentTestVerifier\\CodeSignTestCisco",
        ICodesignVerifier::CISCO_SIGNER_LIST,
        SIGTYPE_DEFAULT ), CodesignStatus::CODE_SIGNER_SUCCESS );
}

TEST_F( ComponentTestCodeSignVerifier, VerifyMismatch )
{
    EXPECT_EQ( m_patient->Verify(
        L"C:\\Windows\\System32\\vcruntime140.dll",
        SIGNER_MOZILLA,
        SIGTYPE_DEFAULT ), CodesignStatus::CODE_SIGNER_VERIFICATION_FAILED );
}

// RD - 1/10/2023 The signer changed on vcruntime140.dll. Disabling test
TEST_F( ComponentTestCodeSignVerifier, DISABLED_VerifyMicrosoft )
{
    EXPECT_EQ( m_patient->Verify(
        L"C:\\Windows\\System32\\vcruntime140.dll",
        SIGNER_MICROSOFT,
        SIGTYPE_DEFAULT ), CodesignStatus::CODE_SIGNER_SUCCESS );
}

TEST_F( ComponentTestCodeSignVerifier, VerifyUnsigned )
{
    EXPECT_EQ( m_patient->Verify(
        L"C:\\Windows\\notepad.exe",
        SIGNER_CISCO_CN,
        SIGTYPE_DEFAULT ), CodesignStatus::CODE_SIGNER_VERIFICATION_FAILED );
}
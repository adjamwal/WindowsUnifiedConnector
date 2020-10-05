#include "pch.h"
#include "PackageManagerInternalModuleAPI.h"
#include "IUcLogger.h"

class ComponentTestControlModule : public ::testing::Test
{
protected:
    void SetUp()
    {
        EXPECT_EQ( CreateModuleInstance( &m_patient, NULL ), PM_MODULE_SUCCESS );
    }

    void TearDown()
    {
        EXPECT_EQ( ReleaseModuleInstance( &m_patient ), PM_MODULE_SUCCESS );
    }

    PM_MODULE_CTX_T m_patient;
};

TEST_F( ComponentTestControlModule, CanStartPM )
{
    EXPECT_EQ( m_patient.fpStart( L".", L".", L"." ), PM_MODULE_SUCCESS );

    Sleep( 100 );
    
    m_patient.fpStop();
}

TEST_F( ComponentTestControlModule, CanStopPM )
{
    EXPECT_EQ( m_patient.fpStart( L".", L".", L"." ), PM_MODULE_SUCCESS );

    Sleep( 100 );

    EXPECT_EQ( m_patient.fpStop(), PM_MODULE_SUCCESS );
}
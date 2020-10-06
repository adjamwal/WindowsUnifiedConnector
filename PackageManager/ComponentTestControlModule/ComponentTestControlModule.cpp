#include "pch.h"
#include "PackageManagerInternalModuleAPI.h"
#include "IUcLogger.h"
#include <cstdio>

class ComponentTestControlModule : public ::testing::Test
{
protected:
    void SetUp()
    {
        FILE* fp = fopen( "config.json", "w" );
        std::string contents = R"(
{
    "cloud": {
        "CheckinUri": "https://packagemanager.cisco.com/checkin",
        "CheckinInterval": 1000
    }
}
)";
        fwrite( contents.c_str(), 1, contents.length(), fp );
        fclose( fp );
        EXPECT_EQ( CreateModuleInstance( &m_patient, NULL ), PM_MODULE_SUCCESS );
    }

    void TearDown()
    {
        DeleteFile( L"config.json" );
        EXPECT_EQ( ReleaseModuleInstance( &m_patient ), PM_MODULE_SUCCESS );
    }

    PM_MODULE_CTX_T m_patient;
};

TEST_F( ComponentTestControlModule, CanStartPM )
{
    EXPECT_EQ( m_patient.fpStart( L".", L".", L"config.json" ), PM_MODULE_SUCCESS );

    Sleep( 100 );
    
    m_patient.fpStop();
}

TEST_F( ComponentTestControlModule, CanStopPM )
{
    EXPECT_EQ( m_patient.fpStart( L".", L".", L"." ), PM_MODULE_SUCCESS );

    Sleep( 100 );

    EXPECT_EQ( m_patient.fpStop(), PM_MODULE_SUCCESS );
}
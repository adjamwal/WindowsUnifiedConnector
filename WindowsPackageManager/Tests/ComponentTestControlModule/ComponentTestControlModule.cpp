#include "pch.h"
#include "PackageManagerInternalModuleAPI.h"
#include "IUcLogger.h"
#include <cstdio>
#include <Windows.h>

typedef PM_MODULE_RESULT_T ( *CreateModule )( PM_MODULE_CTX_T* pPM_MODULE_CTX );
typedef PM_MODULE_RESULT_T ( *ReleaseModule ) ( PM_MODULE_CTX_T* pPM_MODULE_CTX );

class ComponentTestControlModule : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_controlLib = LoadLibrary( L"PM_MCP.dll" );
        if( !m_controlLib ) {
            printf("Unable to load %s. Errno=%lu\n", "PM_MCP", GetLastError() );
        }
        else {
            m_createModule = ( CreateModule )GetProcAddress( m_controlLib, "CreateModuleInstance" );
            m_releaseModule = ( ReleaseModule )GetProcAddress( m_controlLib, "ReleaseModuleInstance" );

            EXPECT_NE( m_createModule, ( CreateModule )NULL );
            EXPECT_NE( m_releaseModule, ( ReleaseModule )NULL );

            FILE* fp = NULL;
            fopen_s( &fp, "config.json", "w" );
            std::string contents = R"(
{
    "cloud": {
        "CheckinUri": "https://packagemanager.cisco.com/checkin",
        "CheckinInterval": 2000,
		"MaxStartupDelay": 2000
    }
}
)";
            fwrite( contents.c_str(), 1, contents.length(), fp );
            fclose( fp );
            EXPECT_EQ( m_createModule( &m_patient ), PM_MODULE_SUCCESS );
        }
    }

    void TearDown()
    {
        ::DeleteFile( L"config.json" );
        EXPECT_EQ( m_releaseModule( &m_patient ), PM_MODULE_SUCCESS );

        if( m_controlLib && FreeLibrary( m_controlLib ) )
        {
            m_controlLib = NULL;
        }
    }

    PM_MODULE_CTX_T m_patient;
    HMODULE m_controlLib;
    CreateModule m_createModule;
    ReleaseModule m_releaseModule;
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

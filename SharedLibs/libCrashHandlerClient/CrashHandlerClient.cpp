#include "CrashHandlerClient.h"
#include <signal.h>
#include <thread>
#include <memory>
#include "IUcLogger.h"
#include "WindowsUtilities.h"

#define OOP_THREAD_TIMEOUT          60000
#define IMN_REG_KEY                 L"SOFTWARE\\Immunet Protect"
#define IMN_REG_PATH_CRASH_PIPE     L"crash_pipe"

google_breakpad::ExceptionHandler* CrashHandlerClient::s_ExceptionHandler = NULL;
std::mutex CrashHandlerClient::s_mutex;

bool MinidumpCompletedCallback( const wchar_t* dump_path,
    const wchar_t* minidump_id,
    void* context,
    EXCEPTION_POINTERS* exinfo,
    MDRawAssertionInfo* assertion,
    bool succeeded )
{
    std::lock_guard<std::mutex> guard( CrashHandlerClient::s_mutex );
    if( context ) {
        CrashHandlerClient* client = ( CrashHandlerClient* )context;
        client->CrashHandled( dump_path, minidump_id );
    }

    return succeeded;
}


CrashHandlerClient::CrashHandlerClient( CrashDumpWrittenCallback callback ) :
    m_DumpDir( L"" ),
    m_DumpFile( L"" ),
    m_CheckThread( NULL ),
    m_callback( callback ),
    m_DumpType( MiniDumpNormal ),
    m_hShutDown( CreateEvent( NULL, TRUE, FALSE, NULL ) )
{

}

CrashHandlerClient::~CrashHandlerClient()
{
    SetEvent( m_hShutDown );

    if( m_CheckThread ) {
        m_CheckThread->join();

        delete( m_CheckThread );
    }

    RemoveCrashHandler();
    CloseHandle( m_hShutDown );
    m_hShutDown = NULL;
}

void CrashHandlerClient::Init( LPCTSTR pDumpFile, MINIDUMP_TYPE dumpType )
{
    m_DumpDir = pDumpFile;
    m_DumpType = ( int )dumpType != -1 ? dumpType : MiniDumpNormal;
}

google_breakpad::ExceptionHandler* CrashHandlerClient::SetupCrashHandlerInternal(const std::wstring& pipeName)
{
    try
    {
        return new google_breakpad::ExceptionHandler(
            m_DumpDir,
            NULL,
            &MinidumpCompletedCallback,
            this,
            google_breakpad::ExceptionHandler::HANDLER_ALL,
            m_DumpType,
            pipeName.c_str(),
            NULL );
    }
    catch( ... )
    {
        return NULL;
    }
}
void HandleAborts( int signal_number )
{
    std::lock_guard<std::mutex> guard( CrashHandlerClient::s_mutex );
    if( CrashHandlerClient::s_ExceptionHandler )
        CrashHandlerClient::s_ExceptionHandler->WriteMinidump();
}

bool CrashHandlerClient::SetupCrashHandler()
{
    std::lock_guard<std::mutex> guard( CrashHandlerClient::s_mutex );
    WLOG_DEBUG( L": Dir %s", m_DumpDir.c_str() );
    if( !m_DumpDir.length() ) {
        return FALSE;
    }

    std::wstring pipeName(L"");
    if( !WindowsUtilities::ReadRegistryStringW( HKEY_LOCAL_MACHINE, IMN_REG_KEY, IMN_REG_PATH_CRASH_PIPE, pipeName ) || !pipeName.length() )
    {
        WLOG_ERROR( L": CSCMS crash pipe name could not be read from AMP registry" );
        return FALSE;
    }

    s_ExceptionHandler = SetupCrashHandlerInternal( pipeName );
    signal( SIGABRT, &HandleAborts );

    if( !s_ExceptionHandler || !s_ExceptionHandler->IsOutOfProcess() ) {
        WLOG_ERROR( L": OOP Not created. Crashes will be handled locally" );
    }
    else {
        WLOG_DEBUG( L": OOP Crash Handler Created" );
    }

    return TRUE;
}

bool CrashHandlerClient::RemoveCrashHandler()
{
    std::lock_guard<std::mutex> guard( CrashHandlerClient::s_mutex );
    signal( SIGABRT, SIG_DFL );
    return TRUE;
}

void CrashHandlerClient::CrashHandled( const wchar_t* dump_path, const wchar_t* minidump_id )
{
    std::lock_guard<std::mutex> guard( CrashHandlerClient::s_mutex );
    WLOG_ERROR( L": Crash Dump Created: %s %s", dump_path, minidump_id );

    if( !s_ExceptionHandler || !s_ExceptionHandler->IsOutOfProcess() ) {
        WLOG_ERROR( L": Crash was not created OOP. Dump not updated" );
    }

    if( m_callback ) {
        m_callback();
    }
}

void CrashHandlerClient::SetDumpFileName( const wchar_t* dump_path,
    const wchar_t* minidump_id )
{
    if( dump_path && minidump_id ) {
        m_DumpFile = dump_path;
        m_DumpFile += L"\\";
        m_DumpFile += minidump_id;
        m_DumpFile += L".dmp";
    }
}
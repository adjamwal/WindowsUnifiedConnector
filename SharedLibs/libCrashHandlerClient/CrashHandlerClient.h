#pragma once

#include <Windows.h>
#include "client/windows/handler/exception_handler.h"
#include <mutex>
#include <string>

class CrashHandlerClient
{
public:
    typedef void( *CrashDumpWrittenCallback )( void );

    CrashHandlerClient( CrashDumpWrittenCallback callback );
    ~CrashHandlerClient();

    void Init( LPCTSTR pDumpFile, MINIDUMP_TYPE dumpType );

    bool SetupCrashHandler();
    bool RemoveCrashHandler();
    void CrashHandled( const wchar_t* dump_path, const wchar_t* minidump_id );

    static google_breakpad::ExceptionHandler* s_ExceptionHandler;
    static std::mutex s_mutex;
private:
    std::wstring m_DumpDir;
    std::wstring m_DumpFile;
    MINIDUMP_TYPE m_DumpType;

    std::thread* m_CheckThread;
    HANDLE m_hShutDown;
    CrashDumpWrittenCallback m_callback;

    google_breakpad::ExceptionHandler* SetupCrashHandlerInternal( const std::wstring& pipeName );

    void SetDumpFileName( const wchar_t* dump_path, const wchar_t* minidump_id );
};
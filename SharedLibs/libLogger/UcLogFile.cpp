#include "pch.h"
#include "UcLogFile.h"
#include <Windows.h>
#include <processenv.h>
#include <set>
#include <initguid.h>
#include <KnownFolders.h>
#include <ShlObj.h>

UcLogFile::UcLogFile() :
    m_logFileName( L"" )
    , m_file( NULL )
    , m_lastTick( 0 )
    , m_maxFileSize( DEFAULT_MAX_FILE_SIZE )
    , m_maxLogFiles( DEFAULT_MAX_LOGFILES )
{

}

UcLogFile::~UcLogFile()
{
    Deinit();
}

void UcLogFile::WriteLogLine( const char* logLevel, const char* logLine )
{
    Init(); // Takes mutex

    std::lock_guard<std::mutex> lock( m_mutex );

    if( m_file ) {
        char tstr[ 32 ] = {};
        DWORD tid = GetCurrentThreadId();
        time_t now = time( NULL );
        struct tm tm;

        localtime_s( &tm, &now );
        strftime( tstr, sizeof( tstr ), "%b %d %H:%M:%S", &tm );

        uint32_t dwTickNow = GetTickCount();

        int iBytesWritten = fprintf( m_file, "(%d, +%d ms) %s [%d] %s: %s\n", dwTickNow, ( dwTickNow - m_lastTick ), tstr, tid, logLevel, logLine );
        m_lastTick = dwTickNow;

        if( iBytesWritten < 0 ) {
            // well, if we can't log to file we can only resort to logging to stderr
            ( void )fprintf( stderr, "failed to log (%s)", logLine );
        }
        else {
            fflush( m_file );
            RotateLogs();
        }
    }
}

void UcLogFile::WriteLogLine( const wchar_t* logLevel, const wchar_t* logLine )
{
    Init(); // Takes mutex

    std::lock_guard<std::mutex> lock( m_mutex );

    if( m_file ) {
        wchar_t tstr[ 32 ] = {};
        DWORD tid = GetCurrentThreadId();
        time_t now = time( NULL );
        struct tm tm;

        localtime_s( &tm, &now );
        wcsftime( tstr, sizeof( tstr ) / sizeof( wchar_t), L"%b %d %H:%M:%S", &tm );

        uint32_t dwTickNow = GetTickCount();

        int iBytesWritten = fwprintf( m_file, L"(%d, +%d ms) %s [%d] %s: %s\n", dwTickNow, ( dwTickNow - m_lastTick ), tstr, tid, logLevel, logLine );
        m_lastTick = dwTickNow;

        if( iBytesWritten < 0 ) {
            // well, if we can't log to file we can only resort to logging to stderr
            ( void )fwprintf( stderr, L"failed to log (%s)", logLine );
        }
        else {
            fflush( m_file );
            RotateLogs();
        }
    }
}

void UcLogFile::Init( const wchar_t* logname )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_file ) {
        m_logFileName = GenerateFileName( logname );

        CreateLogFile();
    }
}

void UcLogFile::SetLogConfig( uint32_t fileSize, uint32_t logFiles )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_maxFileSize = fileSize;
    m_maxLogFiles = logFiles;
}

void UcLogFile::Deinit()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( m_file ) {
        fclose( m_file );
        m_file = NULL;
    }
}

std::wstring UcLogFile::GetProgramDataFolder()
{
    PWSTR path = NULL;
    std::wstring programData;

    HRESULT hr = SHGetKnownFolderPath( FOLDERID_ProgramData, 0, NULL, &path );

    if( SUCCEEDED( hr ) ) {
        programData = path;
        CoTaskMemFree( path );
        path = NULL;
    }

    return programData;
}

std::filesystem::path UcLogFile::GenerateFileName( const wchar_t* logname )
{
    std::filesystem::path filename;

    if( logname ) {
        filename = logname;
    }
    else {
        WCHAR swPath[ MAX_PATH + 5 ] = { 0 };
        DWORD dwSize = GetModuleFileName( NULL, swPath, MAX_PATH );
        std::wstring modulePath = swPath;

        filename = GetProgramDataFolder();
        filename /= L"Cisco";
        filename /= L"UC";

        if( dwSize && ( modulePath.find_last_of( '\\' ) != std::wstring::npos ) ) {
            modulePath = modulePath.substr( modulePath.find_last_of( '\\' ) + 1 );
            filename /= modulePath;
        }
        else {
            filename /= L"UnknownApplication";
        }

        filename += L".log";
    }

    return filename;
}

bool UcLogFile::CreateLogFile()
{
    if( ( m_file == NULL ) && !m_logFileName.empty() ) {
        if( !std::filesystem::exists( m_logFileName.parent_path() ) ) {
            std::filesystem::create_directories( m_logFileName.parent_path() );
        }
        m_file = _wfsopen( m_logFileName.c_str(), L"a+", SH_DENYNO );
    }

    return m_file ? true : false;
}

void UcLogFile::RotateLogs()
{
    if( m_file ) {
        struct _stat64 file_stat = { 0 };

        if( ( _fstat64( _fileno( m_file ), &file_stat ) == 0 ) && ( file_stat.st_size >= m_maxFileSize ) ) {
            if( fclose( m_file ) == 0 ) {  // Rotate only if we successfully close the file
                struct tm tm;
                WCHAR tstr[ 64 ];
                time_t now;
                std::wstring archiveFilePath;
                BOOL bRotateSucceeded = FALSE;

                archiveFilePath = m_logFileName.c_str();
                archiveFilePath = archiveFilePath.substr( 0, archiveFilePath.length() - 4 ); // strip off .log

                now = time( NULL );
                localtime_s( &tm, &now );

                wcsftime( tstr, sizeof( tstr ) / sizeof( WCHAR ), L"_%Y%m%d_%H%M%S.log", &tm );

                archiveFilePath += tstr;
                if( MoveFile( m_logFileName.c_str(), archiveFilePath.c_str() ) ) {
                    bRotateSucceeded = TRUE;
                }
                else if( CopyFile( m_logFileName.c_str(), archiveFilePath.c_str(), TRUE ) ) {
                    bRotateSucceeded = TRUE;
                }

                m_file = _wfsopen( m_logFileName.c_str(), L"w+", SH_DENYNO );
                if( bRotateSucceeded ) {
                    CleanLogs();
                }
            }
        }
    }
}

void UcLogFile::CleanLogs()
{
    std::set<std::wstring> logFileSet;

    std::wstring strSearchPath = m_logFileName.c_str();
    strSearchPath = strSearchPath.substr( 0, strSearchPath.length() - 4 ); // strip off .log
    strSearchPath.append( L"_*" );

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile( strSearchPath.c_str(), &FindFileData );

    if( hFind != INVALID_HANDLE_VALUE ) {
        do {
            logFileSet.insert( FindFileData.cFileName );
        } while( FindNextFile( hFind, &FindFileData ) != 0 );

        if( hFind ) FindClose( hFind );
    }

    while( logFileSet.size() > m_maxLogFiles ) {
        std::set<std::wstring>::iterator it;
        it = logFileSet.begin();

        std::wstring strFilePath( m_logFileName );
        size_t pos = strFilePath.find_last_of( L"\\" );

        if( pos != std::wstring::npos )
            strFilePath = strFilePath.substr( 0, pos + 1 );

        strFilePath.append( *it );
        DeleteFile( strFilePath.c_str() );
        logFileSet.erase( it );
    }
}

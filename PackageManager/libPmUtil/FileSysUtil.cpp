#include "FileSysUtil.h"
#include "PmLogger.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include "IUtf8PathVerifier.h"

struct FileUtilHandle
{
    FILE* file;
};

#ifdef __unix
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL
#endif

FileSysUtil::FileSysUtil( IUtf8PathVerifier& utf8PathVerifier ) :
    m_utf8PathVerifier( utf8PathVerifier )
{

}

FileSysUtil::~FileSysUtil()
{

}

std::string FileSysUtil::ReadTextFile( const std::filesystem::path& filePath )
{
    std::stringstream stream;

    if( !m_utf8PathVerifier.IsPathValid( filePath ) ) {
        WLOG_ERROR( L"path is invalid" );
    } 
    else if( FileExists( filePath ) ) {
        std::ifstream file( filePath );
        if( file.is_open() ) {
            stream << file.rdbuf();
        }
        else {
            LOG_ERROR( "Failed to open file %s", filePath.generic_u8string().c_str() );
        }
    }
    else {
        LOG_ERROR( "filename %s not found", filePath.generic_u8string().c_str() );
    }

    return stream.str();
}

bool FileSysUtil::WriteLine( const std::filesystem::path& filePath, const std::string& data )
{
    bool ret = false;

    if( !m_utf8PathVerifier.IsPathValid( filePath ) ) {
        WLOG_ERROR( L"path is invalid" );
    }
    else {
        std::ofstream file( filePath, std::ios_base::app );

        if( file.is_open() ) {
            file << data << "\n";
            file.close();
            ret = true;
        }
    }

    return ret;
}

std::vector<std::string> FileSysUtil::ReadFileLines( const std::filesystem::path& filePath )
{
    std::vector<std::string> lines;

    if( !m_utf8PathVerifier.IsPathValid( filePath ) ) {
        WLOG_ERROR( L"path is invalid" );
    }
    else {
        std::ifstream file( filePath );

        if( file.is_open() ) {
            std::string str;

            while( std::getline( file, str ) ) {
                lines.push_back( str );
            }

            file.close();
        }
    }

    return lines;
}

FileUtilHandle* FileSysUtil::PmCreateFile( const std::filesystem::path& filePath )
{
    FileUtilHandle* handle = NULL;

    if( filePath.empty() ) {
        LOG_ERROR( "filename is empty" );
    }
    else if( !m_utf8PathVerifier.IsPathValid( filePath ) ) {
        WLOG_ERROR( L"path is invalid" );
    }
    else {
        ::std::filesystem::create_directories( filePath.parent_path() );

        handle = ( FileUtilHandle* )malloc( sizeof( FileUtilHandle ) );
        errno_t rtn = fopen_s( &handle->file, filePath.generic_u8string().c_str(), "wb" );
        if( rtn != 0 ) {
            LOG_ERROR( "fopen_s failed" );
        }
        else {
            LOG_DEBUG( "Created file %s", filePath.generic_u8string().c_str() );
        }
    }

    return handle;
}

int32_t FileSysUtil::CloseFile( FileUtilHandle* handle )
{
    int32_t rtn = -1;

    if( !handle || handle->file == NULL ) {
        WLOG_ERROR( L"Invalid file handle" );
    }
    else {
        rtn = fclose( handle->file );
        if( rtn != 0 ) {
            WLOG_ERROR( L"failed to close handle %x", handle );
        }
    }

    return rtn;
}

size_t FileSysUtil::AppendFile( FileUtilHandle* handle, void* data, size_t dataLen )
{
    size_t bytesWritten = 0;

    if( !handle || handle->file == NULL ) {
        WLOG_ERROR( L"Invalid file handle" );
    }
    else {
        bytesWritten = fwrite( data, 1, dataLen, handle->file );
        if( bytesWritten != dataLen ) {
            WLOG_ERROR( L"fwrite failed. Wrote %d bytes. Expected %d", bytesWritten, dataLen );
        }
    }

    return bytesWritten;
}

std::filesystem::path FileSysUtil::GetTempDir()
{
    auto path = std::filesystem::temp_directory_path();

    //make_preferred will use the preferred separator for the operating system
    // "//" for windows "/" for linux 
    //path.make_preferred();

    // string() will return the path with the preferred separator
    return path;
}

int32_t FileSysUtil::EraseFile( const std::filesystem::path& filePath )
{
    int32_t rtn = -1;
 
    if( !m_utf8PathVerifier.IsPathValid( filePath ) ) {
        WLOG_ERROR( L"path is invalid" );
        return rtn;
    }

    try {
        if( FileExists( filePath ) && ::std::filesystem::remove( filePath ) )
        {
            rtn = 0;
        }
    }
    catch( std::filesystem::filesystem_error& ex ) {
        LOG_ERROR( "%s", ex.what() );
    }

    return rtn;
}

int32_t FileSysUtil::Rename( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
{
    int32_t rtn = -1;

    if( !m_utf8PathVerifier.IsPathValid( oldFilename ) || !m_utf8PathVerifier.IsPathValid( newName ) ) {
        WLOG_ERROR( L"path is invalid" );
        return rtn;
    }

    try {
        ::std::filesystem::create_directories( newName.parent_path() );
        ::std::filesystem::rename( oldFilename, newName );
        rtn = 0;
    }
    catch( std::filesystem::filesystem_error& ex ) {
        LOG_ERROR( "%s", ex.what() );
    }

    return rtn;
}

bool FileSysUtil::FileExists( const std::filesystem::path& filename )
{
    if( !m_utf8PathVerifier.IsPathValid( filename ) ) {
        WLOG_ERROR( L"path is invalid" );
        return false;
    }

    return ::std::filesystem::exists( filename );
}

uint64_t FileSysUtil::FileSize( const std::filesystem::path& filename )
{
    uint64_t rtn = 0;

    if( !m_utf8PathVerifier.IsPathValid( filename ) ) {
        WLOG_ERROR( L"path is invalid" );
        return rtn;
    }

    try {
        if( !filename.empty() && FileExists( filename ) )
        {
            rtn = ::std::filesystem::file_size( filename );
        }
    }
    catch( std::filesystem::filesystem_error& ex ) {
        LOG_ERROR( "%s", ex.what() );
    }

    return rtn;
}

std::filesystem::file_time_type FileSysUtil::FileTime( const std::filesystem::path& filename )
{
    std::filesystem::file_time_type rtn; //=0

    if( !m_utf8PathVerifier.IsPathValid( filename ) ) {
        WLOG_ERROR( L"path is invalid" );
        return rtn;
    }

    try
    {
        if( !filename.empty() && FileExists( filename ) )
        {
            rtn = std::filesystem::last_write_time( filename );
        }
    }
    catch( std::filesystem::filesystem_error& ex ) {
        LOG_ERROR( "%s", ex.what() );
    }
    return rtn;
}

std::string FileSysUtil::AppendPath( const std::string& basePath, const std::string& configPath )
{
    std::filesystem::path path;

    if( configPath.empty() ) {
        WLOG_ERROR( L"configPath is empty" );
    }
    else if( basePath.empty() ) {
         path = configPath;
    }
    else {
        path = basePath;
        path /= configPath;
    }

    if( !m_utf8PathVerifier.IsPathValid( path ) ) {
        WLOG_ERROR( L"path is invalid" );
        return "";
    }

    path.make_preferred();

    LOG_DEBUG( "Path resolved to %s", path.generic_u8string().c_str() );

    return path.generic_u8string();
}

time_t FileSysUtil::LastWriteTime( const std::filesystem::path& filename )
{
    time_t rtn = -1;

    if( !m_utf8PathVerifier.IsPathValid( filename ) ) {
        WLOG_ERROR( L"path is invalid" );
        return rtn;
    }

    struct _stat64 fileInfo;
    if ( _stati64( filename.generic_u8string().c_str(), &fileInfo ) != 0 ) {
        LOG_ERROR( "_stati64 failed on file %s", filename.generic_u8string().c_str() );
    }
    else {
        rtn = fileInfo.st_mtime;
    }

    return rtn;
}

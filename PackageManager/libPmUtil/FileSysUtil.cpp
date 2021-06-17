#include "FileSysUtil.h"
#include "PMLogger.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>

struct FileUtilHandle
{
    FILE* file;
};

FileSysUtil::FileSysUtil()
{

}

FileSysUtil::~FileSysUtil()
{

}

std::string FileSysUtil::ReadFile( const std::filesystem::path& filePath )
{
    std::stringstream stream;

    if( FileExists( filePath ) ) {
        std::ifstream file( filePath );
        if( file.is_open() ) {
            stream << file.rdbuf();
        }
        else {
            LOG_ERROR( "Failed to open file %s", filePath.generic_string().c_str() );
        }
    }
    else {
        LOG_ERROR( "filename is empty" );
    }

    return stream.str();
}

bool FileSysUtil::WriteLine( const std::filesystem::path& filePath, const std::string& data )
{
    bool ret = false;
    std::ofstream file( filePath, std::ios_base::app );

    if ( file.is_open() )
    {
        file << data << "\n";
        file.close();
        ret = true;
    }

    return ret;
}

std::vector<std::string> FileSysUtil::ReadFileLines( const std::filesystem::path& filePath )
{
    std::vector<std::string> lines;
    std::ifstream file( filePath );

    if ( file.is_open() )
    {
        std::string str;

        while ( std::getline( file, str ) )
        {
            lines.push_back( str );
        }

        file.close();
    }
    
    return lines;
}

FileUtilHandle* FileSysUtil::PmCreateFile( const std::filesystem::path& filePath )
{
    FileUtilHandle* handle = NULL;

    if( filePath.empty() ) {
        LOG_ERROR( "filename is empty" );
    }
    else {
        ::std::filesystem::create_directories( filePath.parent_path() );

        handle = ( FileUtilHandle* )malloc( sizeof( FileUtilHandle ) );
        errno_t rtn = fopen_s( &handle->file, filePath.generic_string().c_str(), "wb" );
        if( rtn != 0 ) {
            LOG_ERROR( "fopen_s failed" );
        }
        else {
            LOG_DEBUG( "Created file %s", filePath.generic_string().c_str() );
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

int32_t FileSysUtil::AppendFile( FileUtilHandle* handle, void* data, size_t dataLen )
{
    int32_t bytesWritten = 0;

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

    //make_preferred will use the prefered sepeartor for the operating system
    // "//" for windows "/" for linux 
    //path.make_preferred();

    // string() will return the path with the prefered sepeartor
    return path;
}

int32_t FileSysUtil::DeleteFile( const std::filesystem::path& filePath )
{
    int32_t rtn = -1;
 
    try {
        if ( FileExists( filePath ) ) {
            ::std::filesystem::remove( filePath );
            rtn = 0;
        }
    }
    catch ( std::filesystem::filesystem_error& ex ) {
        LOG_ERROR( "%s", ex.what() );
    }

    return rtn;
}

int32_t FileSysUtil::Rename( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
{
    int32_t rtn = -1;

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
    return ::std::filesystem::exists( filename );
}

size_t FileSysUtil::FileSize( const std::filesystem::path& filename )
{
    size_t rtn = 0;

    try {
        if( !filename.empty() && FileExists( filename ) )
        {
            rtn = ::std::filesystem::file_size( filename );
        }
    }
    catch( std::filesystem::filesystem_error ex ) {
        LOG_ERROR( "%s", ex.what() );
    }

    return rtn;
}

std::filesystem::file_time_type FileSysUtil::FileTime( const std::filesystem::path& filename )
{
    std::filesystem::file_time_type rtn; //=0
    try
    {
        if( !filename.empty() && FileExists( filename ) )
        {
            rtn = std::filesystem::last_write_time( filename );
        }
    }
    catch( std::filesystem::filesystem_error ex ) {
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

    path.make_preferred();
    LOG_DEBUG( "Path resolved to %s", path.generic_string().c_str() );

    return path.generic_string();
}

time_t FileSysUtil::LastWriteTime( const std::filesystem::path& filename )
{
    time_t rtn = -1;

    struct _stat64 fileInfo;
    if ( _stati64( filename.generic_string().c_str(), &fileInfo ) != 0 ) {
        LOG_ERROR( "_stati64 failed on file %s", filename.generic_string().c_str() );
    }
    else {
        rtn = fileInfo.st_mtime;
    }

    return rtn;
}

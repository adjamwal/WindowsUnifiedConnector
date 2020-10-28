#include "FileUtil.h"
#include "PMLogger.h"
#include <sstream>
#include <fstream>
#include <filesystem>

struct FileUtilHandle
{
    FILE* file;
};

FileUtil::FileUtil()
{

}

FileUtil::~FileUtil()
{

}

std::string FileUtil::ReadFile( const std::string& filename )
{
    std::stringstream stream;

    if( filename.length() ) {
        std::ifstream file( filename );
        if( file.is_open() ) {
            stream << file.rdbuf();
        }
        else {
            LOG_ERROR( "Failed to open file %s", filename.c_str() );
        }
    }
    else {
        LOG_ERROR( "filename is empty" );
    }

    return stream.str();
}

FileUtilHandle* FileUtil::PmCreateFile( const std::string& filename )
{
    FileUtilHandle* handle = NULL;

    if( filename.empty() ) {
        WLOG_ERROR( L"filename is empty" );
    }
    else {
        handle = ( FileUtilHandle* )malloc( sizeof( FileUtilHandle ) );
        errno_t rtn = fopen_s( &handle->file, filename.c_str(), "wb" );
        if( rtn != 0 ) {
            WLOG_ERROR( L"fopen_s failed" );
        }
        else {
            WLOG_DEBUG( L"Created file %hs", filename.c_str() );
        }
    }

    return handle;
}

int32_t FileUtil::CloseFile( FileUtilHandle* handle )
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

int32_t FileUtil::AppendFile( FileUtilHandle* handle, void* data, size_t dataLen )
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

std::string FileUtil::GetTempDir()
{
    auto path = std::filesystem::temp_directory_path();

    //make_preferred will use the prefered sepeartor for the operating system
    // "//" for windows "/" for linux 
    path.make_preferred();

    // string() will return the path with the prefered sepeartor
    return path.string();
}

int32_t FileUtil::DeleteFile( const std::string& filename )
{
    return ::std::filesystem::remove( ::std::filesystem::path( filename ) ) ? 0 : -1;
}

int32_t FileUtil::Rename( const std::string& oldFilename, const std::string& newDir, const std::string& newName )
{
    int32_t rtn = -1;

    try {
        ::std::filesystem::path target( newDir );
        target /= newName;
        ::std::filesystem::rename( ::std::filesystem::path( oldFilename ), target );
        rtn = 0;
    }
    catch( std::filesystem::filesystem_error ex ) {
        LOG_ERROR( "%s", ex.what() );
    }

    return rtn;
}
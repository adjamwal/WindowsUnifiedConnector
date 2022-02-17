#include "pch.h"
#include "ZlibWrapper.h"
#include "IUcLogger.h"
#include <Windows.h>
#include <filesystem>

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)

class ScopedDirectory
{
public:
    ScopedDirectory( const std::filesystem::path dirPath ) {
        if( std::filesystem::exists( dirPath ) ) {
            if( !std::filesystem::is_directory( dirPath ) ) {
                throw std::runtime_error( __FUNCTION__ ": Not a directory" );
            }
        }
        else {
            std::filesystem::create_directories( dirPath );
        }

        m_prevDir = std::filesystem::current_path();
        std::filesystem::current_path( dirPath );
    }

    ~ScopedDirectory() {
        if( !m_prevDir.empty() ) {
            std::filesystem::current_path( m_prevDir );
        }
    }

private:
    std::filesystem::path m_prevDir;
};

ZlibWrapper::ZlibWrapper() :
    m_zipFile( nullptr )
{

}

ZlibWrapper::~ZlibWrapper()
{

}

bool ZlibWrapper::CreateArchiveFile( const char* fileName )
{
    return CreateArchiveFile( fileName, APPEND_STATUS_CREATE );
}

bool ZlibWrapper::CreateArchiveFile( const char* fileName, int mode )
{
    if ( m_zipFile != nullptr ) {
        LOG_ERROR( "Zipfile is already open" );
        return false;
    }

    m_zipFile = zipOpen2( fileName, mode, NULL, NULL, 0 );
    if ( !m_zipFile ) {
        LOG_ERROR( "Failed to open %s", fileName );
        return false;
    }

    return true;
}

uLong filetime( const char* f, tm_zip* tmzip, uLong* dt )
{
    int ret = 0;

    FILETIME ftLocal;
    HANDLE hFind;
    WIN32_FIND_DATAA  ff32;

    hFind = FindFirstFileA( f, &ff32 );
    if ( hFind != INVALID_HANDLE_VALUE ) {
        FileTimeToLocalFileTime( &( ff32.ftLastWriteTime ), &ftLocal );
        FileTimeToDosDateTime( &ftLocal, ( ( LPWORD )dt ) + 1, ( ( LPWORD )dt ) + 0 );
        FindClose( hFind );
        ret = 1;
    }

    return ret;
}

std::string ZlibWrapper::StripDriveLetter( const char* filename )
{
    std::string strippedFilename = filename ? filename : "";

    size_t tokenFound = strippedFilename.find( ":\\" );
    if ( tokenFound != std::string::npos ) {
        strippedFilename.erase( strippedFilename.begin(), strippedFilename.begin() + tokenFound + 2 );
    }

    tokenFound = strippedFilename.find( ":/" );
    if ( tokenFound != std::string::npos ) {
        strippedFilename.erase( strippedFilename.begin(), strippedFilename.begin() + tokenFound + 2 );
    }

    return strippedFilename;
}

bool ZlibWrapper::AddFileToArchive( const char* filenameinzip )
{
    FILE* fin = NULL;
    int size_read = 0;
    zip_fileinfo zi = { 0 };
    int err = ZIP_OK;
    int size_buf = WRITEBUFFERSIZE;
    Byte buf[ WRITEBUFFERSIZE ];
    int opt_compress_level = Z_DEFAULT_COMPRESSION;
    std::string strippedFileName;

    if ( m_zipFile == nullptr ) {
        LOG_ERROR( "Zipfile is not open" );
        return false;
    }

    if ( filenameinzip == nullptr ) {
        LOG_ERROR( "fileanme is null" );
        return false;
    }

    zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
        zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = 0;
    filetime( filenameinzip, &zi.tmz_date, &zi.dosDate );

    fin = fopen( filenameinzip, "rb" );
    if ( fin == NULL ) {
        LOG_ERROR( "error in opening %s for reading\n", filenameinzip );
        err = ZIP_ERRNO;
        return false;
    }

    strippedFileName = StripDriveLetter( filenameinzip );

    err = zipOpenNewFileInZip3( m_zipFile, strippedFileName.c_str(), &zi,
        NULL, 0, NULL, 0, NULL /* comment*/,
        ( opt_compress_level != 0 ) ? Z_DEFLATED : 0,
        opt_compress_level, 0,
        -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
        NULL, 0 );

    if ( err != ZIP_OK ) {
        LOG_ERROR( "error in opening %s in zipfile\n", filenameinzip );
        fclose( fin );
        fin = NULL;
        return false;
    }

    do {
        err = ZIP_OK;
        size_read = ( int )fread( buf, 1, size_buf, fin );
        if ( size_read < size_buf )
            if ( feof( fin ) == 0 ) {
                LOG_ERROR( "error in reading %s\n", filenameinzip );
                err = ZIP_ERRNO;
            }

        if ( size_read > 0 ) {
            err = zipWriteInFileInZip( m_zipFile, buf, size_read );
            if ( err < 0 ) {
                LOG_ERROR( __FUNCTION__ " in writing %s in the zipfile",
                    filenameinzip );
            }

        }
    } while ( ( err == ZIP_OK ) && ( size_read > 0 ) );

    fclose( fin );
    fin = NULL;

    err = zipCloseFileInZip( m_zipFile );
    if ( err != ZIP_OK ) {
        LOG_ERROR( "in closing %s in the zipfile", filenameinzip );
        return false;
    }

    return true;
}

bool ZlibWrapper::CloseArchiveFile()
{
    if ( m_zipFile == nullptr ) {
        LOG_ERROR( "Zipfile is not open" );
        return false;
    } else {
        zipClose( m_zipFile, NULL );
        m_zipFile = NULL;
    }

    return true;
}

void ZlibWrapper::ChangeFileDate( const char* filename, uLong dosdate, tm_unz tmu_date )
{
    HANDLE hFile;
    FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

    hFile = CreateFileA( filename, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL );
    GetFileTime( hFile, &ftCreate, &ftLastAcc, &ftLastWrite );
    DosDateTimeToFileTime( ( WORD )( dosdate >> 16 ), ( WORD )dosdate, &ftLocal );
    LocalFileTimeToFileTime( &ftLocal, &ftm );
    SetFileTime( hFile, &ftm, &ftLastAcc, &ftm );
    CloseHandle( hFile );
}

int ZlibWrapper::ExtractCurrentFile( unzFile uf, const char* password )
{
    char filename_inzip[ 1024 ] = { 0 };
    int err = UNZ_OK;
    FILE* fout = NULL;
    void* buf = NULL;
    uInt size_buf = 0;

    unz_file_info64 file_info = { 0 };
    uLong ratio = 0;
    err = unzGetCurrentFileInfo64( uf, &file_info, filename_inzip, sizeof( filename_inzip ), NULL, 0, NULL, 0 );

    if( err != UNZ_OK ) {
        LOG_ERROR( "unzGetCurrentFileInfo failed %d", err );
        return err;
    }

    size_buf = WRITEBUFFERSIZE;
    buf = ( void* )malloc( size_buf );
    if( buf == NULL ) {
        LOG_ERROR( "Failed allocating memory of size %d", size_buf );
        return UNZ_INTERNALERROR;
    }

    std::filesystem::path filePath = filename_inzip;
    
    if( !filePath.parent_path().empty() && !std::filesystem::is_directory( filePath.parent_path() )) {
        LOG_DEBUG( "creating directory : %s", filePath.parent_path().generic_u8string().c_str() );
        std::filesystem::create_directories( filePath.parent_path() );
    }

    const char* write_filename;
    write_filename = filename_inzip;

    err = unzOpenCurrentFilePassword( uf, password );
    if( err != UNZ_OK ) {
        LOG_ERROR( "unzOpenCurrentFilePassword failed %d", err );
    }
    else {
        if( fopen_s( &fout, write_filename, "wb" ) != 0 ) {
            LOG_ERROR( "fopen_s failed" );
        }
        else {
            LOG_DEBUG( "Extracting: %s", write_filename );

            do {
                err = unzReadCurrentFile( uf, buf, size_buf );
                if( err < 0 ) {
                    LOG_ERROR( "unzReadCurrentFile failed: %d", err );
                    break;
                }
                if( err > 0 )
                    if( fwrite( buf, err, 1, fout ) != 1 ) {
                        LOG_ERROR( "fwrite failed" );
                        err = UNZ_ERRNO;
                        break;
                    }
            } while( err > 0 );

            fclose( fout );

            if( err == 0 ) {
                ChangeFileDate( write_filename, file_info.dosDate,
                    file_info.tmu_date );
            }

        }

        if( unzCloseCurrentFile( uf ) != UNZ_OK ) {
            LOG_ERROR( "unzCloseCurrentFile failed" );
        }
    }

    free( buf );
    return err;
}

bool ZlibWrapper::ExtractArchive( const char* fileName, const char* destDirectory )
{
    bool rtn = false;

    if( fileName && destDirectory ) {
        unzFile uf = NULL;

        uf = unzOpen64( fileName );

        if( uf != NULL ) {
            uLong i;
            unz_global_info64 gi = { 0 };
            int err = 0;

            err = unzGetGlobalInfo64( uf, &gi );
            if( err != UNZ_OK )
                LOG_ERROR( "unzGetGlobalInfo failed: %d", err );

            
            try {
                ScopedDirectory scopedDir( destDirectory );

                for( i = 0; i < gi.number_entry; i++ ) {
                    err = ExtractCurrentFile( uf, NULL );
                    if( err != UNZ_OK ) {
                        LOG_ERROR( "ExtractCurrentFile failed: %d", err );
                        break;
                    }

                    if( ( i + 1 ) < gi.number_entry ) {
                        if( unzGoToNextFile( uf ) != UNZ_OK ) {
                            LOG_ERROR( "unzGoToNextFile failed: %d", err );
                            break;
                        }
                    }
                }
            }
            catch( std::exception& e ) {
                LOG_ERROR( "Failed to extract %s to %s. %s", fileName, destDirectory, e.what() );
                err = UNZ_ERRNO;
            }

            rtn = (err == 0);
        }
        else {
            LOG_ERROR( "Failed to open %s", fileName );
        }

        unzClose( uf );
    }
    else {
        LOG_ERROR( "Invalid Parameters" );
    }

    return rtn;
}

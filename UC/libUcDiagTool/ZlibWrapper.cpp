#include "pch.h"
#include "ZlibWrapper.h"
#include "IUcLogger.h"
#include <Windows.h>

#define WRITEBUFFERSIZE 262144

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
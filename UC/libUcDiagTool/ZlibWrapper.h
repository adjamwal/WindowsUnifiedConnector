#pragma once

#include "IZlibWrapper.h"
#include "minizip/zip.h"
#include "minizip/unzip.h"
#include <string>

class ZlibWrapper : public IZlibWrapper
{
public:
    ZlibWrapper();
    ~ZlibWrapper();

    bool CreateArchiveFile( const char* fileName ) override;
    bool CreateArchiveFile( const char* fileName, int mode ) override;
    bool AddFileToArchive( const char* filenameinzip ) override;
    bool CloseArchiveFile() override;

    bool ExtractArchive( const char* fileName, const char* destDirectory ) override;
private:
    zipFile m_zipFile;

    std::string StripDriveLetter( const char* filename );
    int ExtractCurrentFile( unzFile uf, const char* password );
    void ChangeFileDate( const char* filename, uLong dosdate, tm_unz tmu_date );
};

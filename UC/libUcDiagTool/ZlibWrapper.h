#pragma once

#include "IZlibWrapper.h"
#include "minizip/zip.h"
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

private:
    zipFile m_zipFile;

    std::string StripDriveLetter( const char* filename );
};

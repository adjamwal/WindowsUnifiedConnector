#pragma once

class IZlibWrapper
{
public:
    virtual ~IZlibWrapper() {}

    // Zip Apis
    virtual bool CreateArchiveFile( const char* fileName ) = 0;
    virtual bool CreateArchiveFile( const char* fileName, int mode ) = 0;
    virtual bool AddFileToArchive( const char* filenameinzip ) = 0;
    virtual bool CloseArchiveFile() = 0;

    // Unzip Apis
    virtual bool ExtractArchive( const char* fileName, const char* destDirectory ) = 0;
};

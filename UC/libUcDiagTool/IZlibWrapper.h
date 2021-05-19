#pragma once

class IZlibWrapper
{
public:
    virtual ~IZlibWrapper() {}

    virtual bool CreateArchiveFile( const char* fileName ) = 0;
    virtual bool CreateArchiveFile( const char* fileName, int mode ) = 0;
    virtual bool AddFileToArchive( const char* filenameinzip ) = 0;
    virtual bool CloseArchiveFile() = 0;
};

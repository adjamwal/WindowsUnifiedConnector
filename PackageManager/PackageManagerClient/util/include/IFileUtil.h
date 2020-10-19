#pragma once

#include <string>

struct FileUtilHandle;

class IFileUtil
{
public:
    IFileUtil() {}
    virtual ~IFileUtil() {}

    virtual std::string ReadFile( const std::string& filename ) = 0;
    virtual FileUtilHandle* PmCreateFile( const std::string& filename ) = 0;
    virtual int32_t CloseFile( FileUtilHandle* handle ) = 0;
    virtual int32_t AppendFile( FileUtilHandle* handle, void* data, size_t dataLen ) = 0;
};
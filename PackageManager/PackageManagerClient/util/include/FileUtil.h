#pragma once

#include "IFileUtil.h"

class FileUtil : public IFileUtil
{
public:
    FileUtil();
    ~FileUtil();

    std::string ReadFile( const std::string& filename ) override;
    FileUtilHandle* PmCreateFile( const std::string& filename ) override;
    int32_t CloseFile( FileUtilHandle* handle ) override;
    int32_t AppendFile( FileUtilHandle* handle, void* data, size_t dataLen ) override;
    std::string GetTempDir() override;
    virtual int32_t DeleteFile( const std::string& filename ) override;
};
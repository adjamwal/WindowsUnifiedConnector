#pragma once

#include "IFileUtil.h"

class FileUtil : public IFileUtil
{
public:
    FileUtil();
    ~FileUtil();

    std::string ReadFile( const std::string& filename ) override;
    bool WriteLine( const std::string& filename, const std::string& data ) override;
    std::vector<std::string> ReadFileLines( const std::string& filename ) override;
    FileUtilHandle* PmCreateFile( const std::string& filename ) override;
    int32_t CloseFile( FileUtilHandle* handle ) override;
    int32_t AppendFile( FileUtilHandle* handle, void* data, size_t dataLen ) override;
    std::string GetTempDir() override;
    int32_t DeleteFile( const std::string& filename ) override;
    int32_t Rename( const std::string& oldFilename, const std::string& newName ) override;
    bool FileExists( const std::string& filename ) override;
    size_t FileSize( const std::string& filename ) override;
    std::filesystem::file_time_type FileTime( const std::string& filename ) override;
    std::string AppendPath( const std::string& basePath, const std::string& configPath ) override;
    time_t LastWriteTime( const std::string& filename ) override;
};
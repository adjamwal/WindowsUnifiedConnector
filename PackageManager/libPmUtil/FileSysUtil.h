#pragma once

#include "IFileSysUtil.h"

//original FileUtil.h name was overlapping with 
//C:\Program Files (x86)\WiX Toolset v3.11\SDK\VS2015\inc\fileutil.h 
//and couldn't be used from installer project
class FileSysUtil : public IFileSysUtil
{
public:
    FileSysUtil();
    ~FileSysUtil();

    std::string ReadFile( const std::filesystem::path& filePath ) override;
    bool WriteLine( const std::filesystem::path& filePath, const std::string& data ) override;
    std::vector<std::string> ReadFileLines( const std::filesystem::path& filePath ) override;
    FileUtilHandle* PmCreateFile( const std::filesystem::path& filePath ) override;
    int32_t CloseFile( FileUtilHandle* handle ) override;
    int32_t AppendFile( FileUtilHandle* handle, void* data, size_t dataLen ) override;
    std::filesystem::path GetTempDir() override;
    int32_t DeleteFile( const std::filesystem::path& filePath ) override;
    int32_t Rename( const std::filesystem::path& oldfilePath, const std::filesystem::path& newFilePath ) override;
    bool FileExists( const std::filesystem::path& filePath ) override;
    size_t FileSize( const std::filesystem::path& filePath ) override;
    std::filesystem::file_time_type FileTime( const std::filesystem::path& filePath ) override;
    std::string AppendPath( const std::string& basePath, const std::string& configPath ) override;
    time_t LastWriteTime( const std::filesystem::path& filePath ) override;
};
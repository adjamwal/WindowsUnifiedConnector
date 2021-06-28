#pragma once

#include <string>
#include <vector>
#include <filesystem>

struct FileUtilHandle;

//original FileUtil.h name was overlapping with 
//C:\Program Files (x86)\WiX Toolset v3.11\SDK\VS2015\inc\fileutil.h 
//and couldn't be used from installer project
class IFileSysUtil
{
public:
    IFileSysUtil() {}
    virtual ~IFileSysUtil() {}

    virtual std::string ReadFile( const std::filesystem::path& filePath ) = 0;
    virtual bool WriteLine( const std::filesystem::path& filePath, const std::string& data ) = 0;
    virtual std::vector<std::string> ReadFileLines( const std::filesystem::path& filePath ) = 0;
    virtual FileUtilHandle* PmCreateFile( const std::filesystem::path& filePath ) = 0;
    virtual int32_t CloseFile( FileUtilHandle* handle ) = 0;
    virtual size_t AppendFile( FileUtilHandle* handle, void* data, size_t dataLen ) = 0;
    virtual std::filesystem::path GetTempDir() = 0;
    virtual int32_t DeleteFile( const std::filesystem::path& filePath ) = 0;
    virtual int32_t Rename( const std::filesystem::path& oldFilePath, const std::filesystem::path& newFfilePath ) = 0;
    virtual bool FileExists( const std::filesystem::path& filePath ) = 0;
    virtual uint64_t FileSize( const std::filesystem::path& filePath ) = 0;
    virtual std::filesystem::file_time_type FileTime( const std::filesystem::path& filePath ) = 0;
    virtual std::string AppendPath( const std::string& basePath, const std::string& configPath ) = 0;
    virtual time_t LastWriteTime( const std::filesystem::path& filePath ) = 0;
};
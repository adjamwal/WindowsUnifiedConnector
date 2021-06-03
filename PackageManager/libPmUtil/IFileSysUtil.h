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

    virtual std::string ReadFile( const std::string& filename ) = 0;
    virtual bool WriteLine( const std::string& filename, const std::string& data ) = 0;
    virtual std::vector<std::string> ReadFileLines( const std::string& filename ) = 0;
    virtual FileUtilHandle* PmCreateFile( const std::string& filename ) = 0;
    virtual int32_t CloseFile( FileUtilHandle* handle ) = 0;
    virtual int32_t AppendFile( FileUtilHandle* handle, void* data, size_t dataLen ) = 0;
    virtual std::string GetTempDir() = 0;
    virtual int32_t DeleteFile( const std::string& filename ) = 0;
    virtual int32_t Rename( const std::string& oldFilename, const std::string& newName ) = 0;
    virtual bool FileExists( const std::string& filename ) = 0;
    virtual size_t FileSize( const std::string& filename ) = 0;
    virtual std::filesystem::file_time_type FileTime( const std::string& filename ) = 0;
    virtual std::string AppendPath( const std::string& basePath, const std::string& configPath ) = 0;
    virtual time_t LastWriteTime( const std::string& filename ) = 0;
};
#pragma once
#include "MocksCommon.h"
#include "IFileSysUtil.h"

class MockFileSysUtil : public IFileSysUtil
{
public:
    MockFileSysUtil();
    ~MockFileSysUtil();

    MOCK_METHOD1( ReadFile, std::string( const std::filesystem::path& ) );
    void MakeReadFileReturn( std::string value );
    void ExpectReadFileNotCalled();

    MOCK_METHOD2( WriteLine, bool( const std::filesystem::path&, const std::string& ) );
    void MakeWriteLineReturn( bool value );
    void ExpectWriteLineNotCalled();

    MOCK_METHOD1( ReadFileLines, std::vector<std::string>( const std::filesystem::path& ) );
    void MakeReadFileLinesReturn( std::vector<std::string> value );
    void ExpectReadFileLinesNotCalled();

    MOCK_METHOD1( PmCreateFile, FileUtilHandle*( const std::filesystem::path& ) );
    void MakePmCreateFileReturn( FileUtilHandle* value );
    void ExpectPmCreateFileNotCalled();

    MOCK_METHOD1( CloseFile, int32_t( FileUtilHandle* ) );
    void MakeCloseFileReturn( int32_t value );
    void ExpectCloseFileNotCalled();

    MOCK_METHOD3( AppendFile, int32_t( FileUtilHandle*, void*, size_t ) );
    void MakeAppendFileReturn( int32_t value );
    void ExpectAppendFileNotCalled();

    MOCK_METHOD0( GetTempDir, std::filesystem::path() );
    void MakeGetTempDirReturn( std::filesystem::path value );
    void ExpectGetTempDirNotCalled();

    MOCK_METHOD1( DeleteFile, int32_t( const std::filesystem::path& ) );
    void MakeDeleteFileReturn( int32_t value );
    void ExpectDeleteFileNotCalled();

    MOCK_METHOD2( Rename, int32_t( const std::filesystem::path&, const std::filesystem::path& ) );
    void MakeRenameReturn( int32_t value );
    void ExpectRenameNotCalled();

    MOCK_METHOD1( FileExists, bool( const std::filesystem::path& ) );
    void MakeFileExistsReturn( bool value );
    void ExpectFileExistsNotCalled();

    MOCK_METHOD1( FileSize, size_t( const std::filesystem::path& ) );
    void MakeFileSizeReturn( size_t value );
    void ExpectFileSizeNotCalled();

    MOCK_METHOD1( FileTime, std::filesystem::file_time_type( const std::filesystem::path& ) );
    void MakeFileTimeReturn( std::filesystem::file_time_type value );
    void ExpectFileTimeNotCalled();

    MOCK_METHOD2( AppendPath, std::string( const std::string&, const std::string& ) );
    void MakeAppendPathReturn( std::string value );
    void ExpectAppendPathIsNotCalled();

    MOCK_METHOD1( LastWriteTime, time_t( const std::filesystem::path& ) );
    void MakeLastWriteTimeReturn( time_t value );
    void ExpectLastWriteTimeIsNotCalled();

private:
    std::string m_defaultString;
};
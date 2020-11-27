#pragma once
#include "MocksCommon.h"
#include "IFileUtil.h"

class MockFileUtil : public IFileUtil
{
public:
    MockFileUtil();
    ~MockFileUtil();

    MOCK_METHOD1( ReadFile, std::string( const std::string& ) );
    void MakeReadFileReturn( std::string value );
    void ExpectReadFileNotCalled();

    MOCK_METHOD2( WriteLine, bool( const std::string&, const std::string& ) );
    void MakeWriteLineReturn( bool value );
    void ExpectWriteLineNotCalled();

    MOCK_METHOD1( ReadFileLines, std::vector<std::string>( const std::string& ) );
    void MakeReadFileLinesReturn( std::vector<std::string> value );
    void ExpectReadFileLinesNotCalled();

    MOCK_METHOD1( PmCreateFile, FileUtilHandle*( const std::string& ) );
    void MakePmCreateFileReturn( FileUtilHandle* value );
    void ExpectPmCreateFileNotCalled();

    MOCK_METHOD1( CloseFile, int32_t( FileUtilHandle* ) );
    void MakeCloseFileReturn( int32_t value );
    void ExpectCloseFileNotCalled();

    MOCK_METHOD3( AppendFile, int32_t( FileUtilHandle*, void*, size_t ) );
    void MakeAppendFileReturn( int32_t value );
    void ExpectAppendFileNotCalled();

    MOCK_METHOD0( GetTempDir, std::string() );
    void MakeGetTempDirReturn( std::string value );
    void ExpectGetTempDirNotCalled();

    MOCK_METHOD1( DeleteFile, int32_t( const std::string& ) );
    void MakeDeleteFileReturn( int32_t value );
    void ExpectDeleteFileNotCalled();

    MOCK_METHOD2( Rename, int32_t( const std::string&, const std::string& ) );
    void MakeRenameReturn( int32_t value );
    void ExpectRenameNotCalled();

    MOCK_METHOD1( FileExists, bool( const std::string& ) );
    void MakeFileExistsReturn( bool value );
    void ExpectFileExistsNotCalled();

    MOCK_METHOD2( AppendPath, std::string( const std::string&, const std::string& ) );
    void MakeAppendPathReturn( std::string value );
    void ExpectAppendPathCalled();

private:
    std::string m_defaultString;
};
#pragma once
#include "MocksCommon.h"
#include "IFileUtil.h"

class MockFileUtil : public IFileUtil
{
public:
    MockFileUtil();
    ~MockFileUtil();

    MOCK_METHOD1( ReadFile, std::string( const std::string& ));
    void MakeReadFileReturn( std::string value );
    void ExpectReadFileNotCalled();

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

    MOCK_METHOD1( DeleteFile, int32_t( const std::string& ) );

private:
    std::string m_defaultString;
};
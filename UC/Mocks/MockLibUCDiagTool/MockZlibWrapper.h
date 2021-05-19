#pragma once
#include "pch.h"
#include "IZlibWrapper.h"

class MockZlibWrapper : public IZlibWrapper
{
public:
    MockZlibWrapper();
    ~MockZlibWrapper();

    MOCK_METHOD1( CreateArchiveFile, bool( const char*) );
    MOCK_METHOD2( CreateArchiveFile, bool( const char*, int ) );
    void MakeCreateArchiveFileReturn( bool result );
    void ExpectCreateArchiveFileNotCalled();

    MOCK_METHOD1( AddFileToArchive, bool( const char* ) );
    void MakeAddFileToArchiveReturn( bool result );
    void ExpectAddFileToArchiveNotCalled();

    MOCK_METHOD0( CloseArchiveFile, bool() );
    void MakeCloseArchiveFileReturn( bool result );
    void ExpectCloseArchiveFileNotCalled();
};

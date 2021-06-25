#include "pch.h"
#include "MockZlibWrapper.h"

MockZlibWrapper::MockZlibWrapper()
{

}

MockZlibWrapper::~MockZlibWrapper()
{

}

void MockZlibWrapper::MakeCreateArchiveFileReturn( bool result )
{
    ON_CALL( *this, CreateArchiveFile( _ ) ).WillByDefault( Return( result ) );
    ON_CALL( *this, CreateArchiveFile( _,_ ) ).WillByDefault( Return( result ) );
}

void MockZlibWrapper::ExpectCreateArchiveFileNotCalled()
{
    EXPECT_CALL( *this, CreateArchiveFile( _ ) ).Times( 0 );
    EXPECT_CALL( *this, CreateArchiveFile( _, _ ) ).Times( 0 );
}

void MockZlibWrapper::MakeAddFileToArchiveReturn( bool result )
{
    ON_CALL( *this, AddFileToArchive( _ ) ).WillByDefault( Return( result ) );
}

void MockZlibWrapper::ExpectAddFileToArchiveNotCalled()
{
    EXPECT_CALL( *this, AddFileToArchive( _ ) ).Times( 0 );
}

void MockZlibWrapper::MakeCloseArchiveFileReturn( bool result )
{
    ON_CALL( *this, CloseArchiveFile() ).WillByDefault( Return( result ) );
}

void MockZlibWrapper::ExpectCloseArchiveFileNotCalled()
{
    EXPECT_CALL( *this, CloseArchiveFile() ).Times( 0 );
}
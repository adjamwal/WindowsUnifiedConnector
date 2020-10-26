#include "MockFileUtil.h"

MockFileUtil::MockFileUtil() :
    m_defaultString( "" )
{
    MakeReadFileReturn( m_defaultString );
    MakeGetTempDirReturn( m_defaultString );
}

MockFileUtil::~MockFileUtil()
{

}

void MockFileUtil::MakeReadFileReturn( std::string value )
{
    ON_CALL( *this, ReadFile( _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectReadFileNotCalled()
{
    EXPECT_CALL( *this, ReadFile( _ ) ).Times( 0 );
}

void MockFileUtil::MakePmCreateFileReturn( FileUtilHandle* value )
{
    ON_CALL( *this, PmCreateFile( _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectPmCreateFileNotCalled()
{
    EXPECT_CALL( *this, PmCreateFile( _ ) ).Times( 0 );
}


void MockFileUtil::MakeCloseFileReturn( int32_t value )
{
    ON_CALL( *this, CloseFile( _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectCloseFileNotCalled()
{
    EXPECT_CALL( *this, CloseFile( _ ) ).Times( 0 );
}

void MockFileUtil::MakeAppendFileReturn( int32_t value )
{
    ON_CALL( *this, AppendFile( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectAppendFileNotCalled()
{
    EXPECT_CALL( *this, AppendFile( _, _, _ ) ).Times( 0 );
}

void MockFileUtil::MakeGetTempDirReturn( std::string value )
{
    ON_CALL( *this, GetTempDir() ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectGetTempDirNotCalled()
{
    EXPECT_CALL( *this, GetTempDir() ).Times( 0 );
}

void MockFileUtil::MakeDeleteFileReturn( int32_t value )
{
    ON_CALL( *this, DeleteFile( _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectDeleteFileNotCalled()
{
    EXPECT_CALL( *this, DeleteFile( _ ) ).Times( 0 );
}

void MockFileUtil::MakeRenameReturn( int32_t value )
{
    ON_CALL( *this, Rename( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectRenameNotCalled()
{
    EXPECT_CALL( *this, Rename( _, _, _ ) ).Times( 0 );
}
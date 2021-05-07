#include "MockFileUtil.h"

MockFileUtil::MockFileUtil() :
    m_defaultString( "" )
{
    MakeReadFileReturn( m_defaultString );
    MakeGetTempDirReturn( m_defaultString );
    MakeAppendPathReturn( m_defaultString );
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

void MockFileUtil::MakeWriteLineReturn( bool value )
{
    ON_CALL( *this, WriteLine( _, _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectWriteLineNotCalled()
{
    EXPECT_CALL( *this, WriteLine( _, _ ) ).Times( 0 );
}

void MockFileUtil::MakeReadFileLinesReturn( std::vector<std::string> value )
{
    ON_CALL( *this, ReadFileLines( _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectReadFileLinesNotCalled()
{
    EXPECT_CALL( *this, ReadFileLines( _ ) ).Times( 0 );
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
    ON_CALL( *this, Rename( _, _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectRenameNotCalled()
{
    EXPECT_CALL( *this, Rename( _, _ ) ).Times( 0 );
}

void MockFileUtil::MakeFileExistsReturn( bool value )
{
    ON_CALL( *this, FileExists ( _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectFileExistsNotCalled()
{
    EXPECT_CALL( *this, FileExists( _ ) ).Times( 0 );
}

void MockFileUtil::MakeFileSizeReturn( size_t value )
{
    ON_CALL( *this, FileSize( _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectFileSizeNotCalled()
{
    EXPECT_CALL( *this, FileSize( _ ) ).Times( 0 );
}

void MockFileUtil::MakeFileTimeReturn( std::filesystem::file_time_type value )
{
    ON_CALL( *this, FileTime( _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectFileTimeNotCalled()
{
    EXPECT_CALL( *this, FileTime( _ ) ).Times( 0 );
}

void MockFileUtil::MakeAppendPathReturn( std::string value )
{
    ON_CALL( *this, AppendPath( _, _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectAppendPathIsNotCalled()
{
    EXPECT_CALL( *this, AppendPath( _, _ ) ).Times( 0 );
}

void MockFileUtil::MakeLastWriteTimeReturn( time_t value )
{
    ON_CALL( *this, LastWriteTime( _ ) ).WillByDefault( Return( value ) );
}

void MockFileUtil::ExpectLastWriteTimeIsNotCalled()
{
    EXPECT_CALL( *this, LastWriteTime( _ ) ).Times( 0 );
}
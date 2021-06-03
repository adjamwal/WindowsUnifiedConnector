#include "MockFileSysUtil.h"

MockFileSysUtil::MockFileSysUtil() :
    m_defaultString( "" )
{
    MakeReadFileReturn( m_defaultString );
    MakeGetTempDirReturn( m_defaultString );
    MakeAppendPathReturn( m_defaultString );
}

MockFileSysUtil::~MockFileSysUtil()
{

}

void MockFileSysUtil::MakeReadFileReturn( std::string value )
{
    ON_CALL( *this, ReadFile( _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectReadFileNotCalled()
{
    EXPECT_CALL( *this, ReadFile( _ ) ).Times( 0 );
}

void MockFileSysUtil::MakeWriteLineReturn( bool value )
{
    ON_CALL( *this, WriteLine( _, _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectWriteLineNotCalled()
{
    EXPECT_CALL( *this, WriteLine( _, _ ) ).Times( 0 );
}

void MockFileSysUtil::MakeReadFileLinesReturn( std::vector<std::string> value )
{
    ON_CALL( *this, ReadFileLines( _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectReadFileLinesNotCalled()
{
    EXPECT_CALL( *this, ReadFileLines( _ ) ).Times( 0 );
}

void MockFileSysUtil::MakePmCreateFileReturn( FileUtilHandle* value )
{
    ON_CALL( *this, PmCreateFile( _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectPmCreateFileNotCalled()
{
    EXPECT_CALL( *this, PmCreateFile( _ ) ).Times( 0 );
}


void MockFileSysUtil::MakeCloseFileReturn( int32_t value )
{
    ON_CALL( *this, CloseFile( _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectCloseFileNotCalled()
{
    EXPECT_CALL( *this, CloseFile( _ ) ).Times( 0 );
}

void MockFileSysUtil::MakeAppendFileReturn( int32_t value )
{
    ON_CALL( *this, AppendFile( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectAppendFileNotCalled()
{
    EXPECT_CALL( *this, AppendFile( _, _, _ ) ).Times( 0 );
}

void MockFileSysUtil::MakeGetTempDirReturn( std::string value )
{
    ON_CALL( *this, GetTempDir() ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectGetTempDirNotCalled()
{
    EXPECT_CALL( *this, GetTempDir() ).Times( 0 );
}

void MockFileSysUtil::MakeDeleteFileReturn( int32_t value )
{
    ON_CALL( *this, DeleteFile( _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectDeleteFileNotCalled()
{
    EXPECT_CALL( *this, DeleteFile( _ ) ).Times( 0 );
}

void MockFileSysUtil::MakeRenameReturn( int32_t value )
{
    ON_CALL( *this, Rename( _, _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectRenameNotCalled()
{
    EXPECT_CALL( *this, Rename( _, _ ) ).Times( 0 );
}

void MockFileSysUtil::MakeFileExistsReturn( bool value )
{
    ON_CALL( *this, FileExists ( _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectFileExistsNotCalled()
{
    EXPECT_CALL( *this, FileExists( _ ) ).Times( 0 );
}

void MockFileSysUtil::MakeFileSizeReturn( size_t value )
{
    ON_CALL( *this, FileSize( _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectFileSizeNotCalled()
{
    EXPECT_CALL( *this, FileSize( _ ) ).Times( 0 );
}

void MockFileSysUtil::MakeFileTimeReturn( std::filesystem::file_time_type value )
{
    ON_CALL( *this, FileTime( _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectFileTimeNotCalled()
{
    EXPECT_CALL( *this, FileTime( _ ) ).Times( 0 );
}

void MockFileSysUtil::MakeAppendPathReturn( std::string value )
{
    ON_CALL( *this, AppendPath( _, _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectAppendPathIsNotCalled()
{
    EXPECT_CALL( *this, AppendPath( _, _ ) ).Times( 0 );
}

void MockFileSysUtil::MakeLastWriteTimeReturn( time_t value )
{
    ON_CALL( *this, LastWriteTime( _ ) ).WillByDefault( Return( value ) );
}

void MockFileSysUtil::ExpectLastWriteTimeIsNotCalled()
{
    EXPECT_CALL( *this, LastWriteTime( _ ) ).Times( 0 );
}
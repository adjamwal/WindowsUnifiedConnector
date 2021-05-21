#include "pch.h"

#include "DiagFileListBuilder.h"
#include "MockWindowsUtilities.h"
#include <memory>

class TestDiagFileListBuilder: public ::testing::Test
{
protected:
    void SetUp()
    {
        MockWindowsUtilities::Init();
        m_windowsUtils = MockWindowsUtilities::GetMockWindowUtilities();

        m_patient.reset( new DiagFileListBuilder() );
    }

    void TearDown()
    {
        m_patient.reset();
        m_windowsUtils = nullptr;
        MockWindowsUtilities::Deinit();
    }

    MockWindowsUtilities* m_windowsUtils;
    std::unique_ptr<DiagFileListBuilder> m_patient;
};

TEST_F( TestDiagFileListBuilder, WillFindFilesFromConfigPath )
{
    std::wstring configPath = L"ValidPath";
    std::vector<std::filesystem::path> expectedList;
    std::vector<std::filesystem::path> actualList;
    expectedList.push_back( "ConfigPath1" );
    expectedList.push_back( "ConfigPath2" );

    ON_CALL( *m_windowsUtils, ReadRegistryString( _, _, _, _ ) ).WillByDefault( DoAll( 
        SetArgReferee<3>( configPath ), 
        Return( true ) ) );

    ON_CALL( *m_windowsUtils, FileSearchWithWildCard( _, _ ) ).WillByDefault( Invoke( 
        [this, &expectedList]( const std::filesystem::path& searchPath, std::vector<std::filesystem::path>& results )
        {
            results.insert( results.end(), expectedList.begin(), expectedList.end() );
            return 0;
        } ) );

    m_patient->GetFileList( actualList );

    EXPECT_EQ( expectedList, actualList );
}

TEST_F( TestDiagFileListBuilder, WillNotSearchWhenConfigPathIsEmpty )
{
    std::wstring configPath;
    std::vector<std::filesystem::path> actualList;

    m_windowsUtils->MakeReadRegistryStringReturn( true );

    m_windowsUtils->ExpectFileSearchWithWildCardIsNotCalled();

    m_patient->GetFileList( actualList );
}

TEST_F( TestDiagFileListBuilder, WillFindFilesFromLogPath )
{
    std::wstring logPath = L"ValidPath";
    std::vector<std::filesystem::path> expectedList;
    std::vector<std::filesystem::path> actualList;
    expectedList.push_back( "LogPath1" );
    expectedList.push_back( "LogPath2" );

    m_windowsUtils->MakeGetLogDirReturn( logPath );

    ON_CALL( *m_windowsUtils, FileSearchWithWildCard( _, _ ) ).WillByDefault( Invoke(
        [this, &expectedList]( const std::filesystem::path& searchPath, std::vector<std::filesystem::path>& results )
        {
            results.insert( results.end(), expectedList.begin(), expectedList.end() );
            return 0;
        } ) );

    m_patient->GetFileList( actualList );

    EXPECT_EQ( expectedList, actualList );
}

TEST_F( TestDiagFileListBuilder, WillNotSearchWhenLogPathIsEmpty )
{
    std::wstring logPath;
    std::vector<std::filesystem::path> actualList;

    m_windowsUtils->MakeGetLogDirReturn( logPath );

    m_windowsUtils->ExpectFileSearchWithWildCardIsNotCalled();

    m_patient->GetFileList( actualList );
}

TEST_F( TestDiagFileListBuilder, WillCreateManifest )
{
    std::wstring logPath = L"ValidPath";
    std::vector<std::filesystem::path> actualList;

    m_windowsUtils->MakeGetLogDirReturn( logPath );

    EXPECT_CALL( *m_windowsUtils, WriteFileContents( HasSubstr( L"manifest.txt" ), _, _ ) ).WillOnce( Return( true ) );

    m_patient->GetFileList( actualList );
}

TEST_F( TestDiagFileListBuilder, WillAddManifestToList )
{
    std::wstring logPath = L"ValidPath";
    std::vector<std::filesystem::path> actualList;

    m_windowsUtils->MakeGetLogDirReturn( logPath );
    m_windowsUtils->MakeWriteFileContentsReturn( true );

    m_patient->GetFileList( actualList );

    EXPECT_EQ( 1, actualList.size() );
}

TEST_F( TestDiagFileListBuilder, WillOnlyAddManifestOnce )
{
    std::wstring logPath = L"ValidPath";
    std::vector<std::filesystem::path> actualList;
    std::vector<std::filesystem::path> logList;
    logList.push_back( "ValidPath/manifest.txt" );

    m_windowsUtils->MakeGetLogDirReturn( logPath );
    m_windowsUtils->MakeWriteFileContentsReturn( true );

    ON_CALL( *m_windowsUtils, FileSearchWithWildCard( _, _ ) ).WillByDefault( Invoke(
        [this, &logList]( const std::filesystem::path& searchPath, std::vector<std::filesystem::path>& results )
        {
            results.insert( results.end(), logList.begin(), logList.end() );
            return 0;
        } ) );

    m_patient->GetFileList( actualList );

    EXPECT_EQ( 1, actualList.size() );
}
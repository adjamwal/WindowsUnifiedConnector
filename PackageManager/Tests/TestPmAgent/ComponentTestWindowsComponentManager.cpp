
#include "MocksCommon.h"
#include "WindowsComponentManager.h"
#include "WinApiWrapper.h"
#include "MockCodesignVerifier.h"
#include "MockPackageDiscovery.h"
#include <iostream>
#include <filesystem>
#include <fstream>

class ComponentTestWindowsComponentManager : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_winApiWrapper.reset( new WinApiWrapper() );
        m_codeSignVerifier.reset( new NiceMock<MockCodesignVerifier>() );
        m_packageDiscovery.reset( new NiceMock<MockPackageDiscovery>() );

        m_patient.reset( new WindowsComponentManager( *m_winApiWrapper, *m_codeSignVerifier, *m_packageDiscovery ) );
    }

    void TearDown()
    {
        std::filesystem::remove_all( m_baseTestPath );

        m_patient.reset();

        m_winApiWrapper.reset();
        m_codeSignVerifier.reset();
        m_packageDiscovery.reset();
    }

    void CreateFiles( std::vector<std::filesystem::path>& files )
    {
        for ( const auto& file : files )
        {
            auto p = file.parent_path();
            std::filesystem::create_directories( p );
            
            std::ofstream ofs( file.string() );
            ofs << "This is a test file.\n";
            ofs.close();
        }
    }

    void ExecuteTestSearch(
        std::filesystem::path path,
        std::vector<std::filesystem::path>& files,
        std::vector<std::filesystem::path>& expected )
    {
        std::vector<std::filesystem::path> discoveredFiles;

        CreateFiles( files );

        int32_t rtn = m_patient->FileSearchWithWildCard( path, discoveredFiles );

        EXPECT_EQ( rtn, 0 );
        EXPECT_EQ( expected.size(), discoveredFiles.size() );

        for ( auto& discoveredFile : discoveredFiles )
        {
            EXPECT_TRUE( std::find( expected.begin(), expected.end(), discoveredFile ) != expected.end() );
        }
    }

    std::filesystem::path m_baseTestPath = "C:\\ProgramData\\Test\\";

    std::unique_ptr<WinApiWrapper> m_winApiWrapper;
    std::unique_ptr<MockCodesignVerifier> m_codeSignVerifier;
    std::unique_ptr<MockPackageDiscovery> m_packageDiscovery;

    std::unique_ptr<WindowsComponentManager> m_patient;
};

TEST_F( ComponentTestWindowsComponentManager, NoFilesToFind )
{
    std::filesystem::path path( m_baseTestPath / "test.xml" );

    std::vector<std::filesystem::path> files;

    std::vector<std::filesystem::path> expected;

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsComponentManager, NoFilesToFindWildcard )
{
    std::filesystem::path path( m_baseTestPath / "*.xml" );

    std::vector<std::filesystem::path> files;

    std::vector<std::filesystem::path> expected;

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsComponentManager, OneFileToFind )
{
    std::filesystem::path path( m_baseTestPath / "test.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "test.xml" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "test.xml" );

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsComponentManager, OneFileToFindWildcard )
{
    std::filesystem::path path( m_baseTestPath / "*.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "test.xml" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "test.xml" );

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsComponentManager, NoFilesFindWildcardMultipleExistsingFiles )
{
    std::filesystem::path path( m_baseTestPath / "*.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "test.txt" );
    files.push_back( m_baseTestPath / "test.json" );

    std::vector<std::filesystem::path> expected;

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsComponentManager, OneFileToFindWildcardMultipleExistingFiles )
{
    std::filesystem::path path( m_baseTestPath / "*.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "test.xml" );
    files.push_back( m_baseTestPath / "test.txt" );
    files.push_back( m_baseTestPath / "test.json" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "test.xml" );

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsComponentManager, SingleCharacterWildcardSearch )
{
    std::filesystem::path path( m_baseTestPath / "policy.???" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "policy.xml" );
    files.push_back( m_baseTestPath / "policy.txt" );
    files.push_back( m_baseTestPath / "policy.json" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "policy.xml" );
    expected.push_back( m_baseTestPath / "policy.txt" );

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsComponentManager, SearchWithDirectoryWildcard )
{
    std::filesystem::path path( m_baseTestPath / "Temp*Path\\test.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "TempPath\\test.xml" );
    files.push_back( m_baseTestPath / "TempPath\\other.xml" );
    files.push_back( m_baseTestPath / "TempNewPath\\test.xml" );
    files.push_back( m_baseTestPath / "TempNewPath\\other.xml" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "TempPath\\test.xml" );
    expected.push_back( m_baseTestPath / "TempNewPath\\test.xml" );

    ExecuteTestSearch( path, files, expected );
}

TEST_F( ComponentTestWindowsComponentManager, SearchWithDirectoryWildcardAndFileWildcard )
{
    std::filesystem::path path( m_baseTestPath / "Temp*Path\\*.xml" );

    std::vector<std::filesystem::path> files;
    files.push_back( m_baseTestPath / "TempPath\\test.xml" );
    files.push_back( m_baseTestPath / "TempPath\\test2.xml" );
    files.push_back( m_baseTestPath / "TempPath\\test.txt" );
    files.push_back( m_baseTestPath / "TempNewPath\\test.xml" );
    files.push_back( m_baseTestPath / "TempNewPath\\test2.xml" );
    files.push_back( m_baseTestPath / "TempNewPath\\test.txt" );

    std::vector<std::filesystem::path> expected;
    expected.push_back( m_baseTestPath / "TempPath\\test.xml" );
    expected.push_back( m_baseTestPath / "TempPath\\test2.xml" );
    expected.push_back( m_baseTestPath / "TempNewPath\\test.xml" );
    expected.push_back( m_baseTestPath / "TempNewPath\\test2.xml" );

    ExecuteTestSearch( path, files, expected );
}
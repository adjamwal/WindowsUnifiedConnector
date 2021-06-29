#include "pch.h"

#include "DiagPackager.h"
#include "MockZlibWrapper.h"
#include <memory>

class TestDiagPackager : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_zlibWrapper.reset( new NiceMock<MockZlibWrapper>() );

        m_patient.reset( new DiagPackager( *m_zlibWrapper ) );
    }

    void TearDown()
    {
        m_patient.reset();

        m_zlibWrapper.reset();
    }

    std::unique_ptr<MockZlibWrapper> m_zlibWrapper;

    std::unique_ptr<DiagPackager> m_patient;
};

TEST_F( TestDiagPackager, InvalidParametersWillThow )
{
    std::vector<std::filesystem::path> fileList;
    std::filesystem::path packagePath;

    EXPECT_THROW( m_patient->CreatePackage( fileList, packagePath ), std::runtime_error );
}

TEST_F( TestDiagPackager, WillCreateArchive )
{
    std::vector<std::filesystem::path> fileList;
    std::filesystem::path packagePath = "ValidPath";

    fileList.push_back( "SomeFile" );

    EXPECT_CALL( *m_zlibWrapper, CreateArchiveFile( _ ) ).WillOnce( Return( true ) );

    m_patient->CreatePackage( fileList, packagePath );
}

TEST_F( TestDiagPackager, CreateArchiveFailureWillThow )
{
    std::vector<std::filesystem::path> fileList;
    std::filesystem::path packagePath = "ValidPath";

    fileList.push_back( "SomeFile" );
    m_zlibWrapper->MakeCreateArchiveFileReturn( false );

    EXPECT_THROW( m_patient->CreatePackage( fileList, packagePath ), std::runtime_error );
}

TEST_F( TestDiagPackager, AddfilesToArchive )
{
    std::vector<std::filesystem::path> fileList;
    std::filesystem::path packagePath = "ValidPath";

    fileList.push_back( "SomeFile1" );
    fileList.push_back( "SomeFile2" );
    fileList.push_back( "SomeFile3" );
    m_zlibWrapper->MakeCreateArchiveFileReturn( true );

    EXPECT_CALL( *m_zlibWrapper, AddFileToArchive( _ ) ).Times( ( int )fileList.size() );

    m_patient->CreatePackage( fileList, packagePath );
}

TEST_F( TestDiagPackager, WillCloseArchive )
{
    std::vector<std::filesystem::path> fileList;
    std::filesystem::path packagePath = "ValidPath";

    fileList.push_back( "SomeFile1" );
    m_zlibWrapper->MakeCreateArchiveFileReturn( true );

    EXPECT_CALL( *m_zlibWrapper, CloseArchiveFile() );

    m_patient->CreatePackage( fileList, packagePath );
}

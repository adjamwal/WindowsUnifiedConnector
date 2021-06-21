#include "MocksCommon.h"
#include "Utf8PathVerifier.h"
#include <memory>
#include <sstream>

class TestUtf8PathVerifier: public ::testing::Test
{
protected:
    void SetUp()
    {
        m_patient.reset( new Utf8PathVerifier() );
    }

    void TearDown()
    {
        m_patient.reset();
    }

    std::unique_ptr<Utf8PathVerifier> m_patient;
};

TEST_F( TestUtf8PathVerifier, WillVerifyAnsiPath )
{
    std::filesystem::path testPath = L"C:/SomeTestPath/test.xml";

    EXPECT_TRUE( m_patient->IsPathValid( testPath) );
}

TEST_F( TestUtf8PathVerifier, WillVerifyUnicodePath )
{
    std::filesystem::path testPath = L"C:/\u0424\u0430\u0439\u043b\u593e/test.xml";

    EXPECT_TRUE( m_patient->IsPathValid( testPath ) );
}

TEST_F( TestUtf8PathVerifier, WillNotVerifyInvalidUnicode )
{
    std::filesystem::path testPath = L"C:/\xD83D/test.xml";

    EXPECT_FALSE( m_patient->IsPathValid( testPath ) );
}

TEST_F( TestUtf8PathVerifier, WillPruneInvalidPaths )
{
    std::vector<std::filesystem::path> pathList;
    pathList.push_back( L"C:/\xD83D/test.xml" );

    m_patient->PruneInvalidPathsFromList( pathList );

    EXPECT_EQ( pathList.size(), 0 );
}

TEST_F( TestUtf8PathVerifier, WillNotPruneValidPaths )
{
    std::vector<std::filesystem::path> pathList;
    pathList.push_back( L"C:/\u0424\u0430\u0439\u043b\u593e/test.xml" );

    m_patient->PruneInvalidPathsFromList( pathList );

    EXPECT_EQ( pathList.size(), 1 );
}

TEST_F( TestUtf8PathVerifier, WillHandleValidAndInvalidPaths )
{
    std::vector<std::filesystem::path> pathList;
    pathList.push_back( L"C:/\xD83D/test.xml" );
    pathList.push_back( L"C:/\u0424\u0430\u0439\u043b\u593e/test.xml" );
    pathList.push_back( L"C:/SomeTestPath/test.xml" );

    m_patient->PruneInvalidPathsFromList( pathList );

    EXPECT_EQ( pathList.size(), 2 );
}

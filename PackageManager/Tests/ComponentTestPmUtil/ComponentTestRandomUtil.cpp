#include "gtest/gtest.h"
#include "RandomUtil.h"

class ComponentTestRandomUtil : public ::testing::Test
{
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

TEST_F( ComponentTestRandomUtil, WillGenerateDifferentInts )
{
    int prevint = 0;
    for( int i = 0; i < 10; i++ )
    {
        uint64_t newint = RandomUtil::GetInt();
        ASSERT_NE( prevint, newint );
        prevint = newint;
    }
}

TEST_F( ComponentTestRandomUtil, WillGenerateDifferentStrings )
{
    std::string prevstr = "";
    for( int i = 0; i < 10; i++ )
    {
        std::string str = RandomUtil::GetString();
        ASSERT_STRNE( prevstr.c_str(), str.c_str() );
        prevstr = str;
    }
}

TEST_F( ComponentTestRandomUtil, WillGenerateDifferentWStrings )
{
    std::wstring prevstr = L"";
    for( int i = 0; i < 10; i++ )
    {
        std::wstring str = RandomUtil::GetWString();
        ASSERT_STRNE( prevstr.c_str(), str.c_str() );
        prevstr = str;
    }
}

TEST_F( ComponentTestRandomUtil, WillGenerateDifferentSeeds )
{
    uint64_t prevseed = 0;
    for( int i = 0; i < 10; i++ )
    {
        uint64_t seed = RandomUtil::GetSeed();
        ASSERT_NE( prevseed, seed );
        prevseed = seed;
    }
}

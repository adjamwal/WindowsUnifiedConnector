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
    int intarr1[ 10 ];
    int intarr2[ 10 ];
    for( int i = 0; i < 10; i++ )
    {
        intarr1[ i ] = RandomUtil::GetInt();
        intarr2[ i ] = RandomUtil::GetInt();
    }

    ASSERT_NE( intarr1, intarr2 );
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

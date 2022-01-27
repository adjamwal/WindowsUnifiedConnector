#include "gtest/gtest.h"
#include <string>
#include <regex>
#include "StringUtil.h"

class TestStringUtil : public ::testing::Test
{
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

TEST_F( TestStringUtil, WillTrimStr )
{
    ASSERT_STREQ( "string", StringUtil::Trim( " string \n" ).c_str() );
}

TEST_F( TestStringUtil, WillTrimWStr )
{
    ASSERT_STREQ( L"wstring", StringUtil::Trim( L" wstring \n" ).c_str() );
}

TEST_F( TestStringUtil, WillConvertStrToWStr )
{
    ASSERT_STREQ( L"converted", StringUtil::Str2WStr( "converted" ).c_str() );
}

TEST_F( TestStringUtil, WillConvertWStrToStr )
{
    ASSERT_STREQ( "converted", StringUtil::WStr2Str( L"converted" ).c_str() );
}

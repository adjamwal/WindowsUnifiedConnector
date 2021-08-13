#include "gtest/gtest.h"
#include <string>
#include <regex>
#include "TimeUtil.h"

class TestTimeUtil : public ::testing::Test
{
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

TEST_F( TestTimeUtil, WillGenerateRFC3339FormattedTimeString )
{
    std::string tstr = TimeUtil::Now_RFC3339();

    std::cout << __FUNCTION__ << ": " << tstr << std::endl;

    std::regex rexp( RFC3339_REGEXP );
    std::smatch match;
    std::regex_search( tstr, match, rexp );

    ASSERT_TRUE( match.ready() );
    ASSERT_EQ( 2, match.size() );
    ASSERT_TRUE( match[ 0 ].matched );
    ASSERT_EQ( 28, match[ 0 ].length() );
}

TEST_F( TestTimeUtil, WillConvertMillisToValidRFC3339 )
{
    auto millis = TimeUtil::Now_MilliTimeStamp();
    std::string tstr = TimeUtil::MillisToRFC3339( millis );

    std::cout << __FUNCTION__ << ": " << tstr << std::endl;

    std::regex rexp( RFC3339_REGEXP );
    std::smatch match;
    std::regex_search( tstr, match, rexp );

    ASSERT_TRUE( match.ready() );
    ASSERT_EQ( 2, match.size() );
    ASSERT_TRUE( match[ 0 ].matched );
    ASSERT_EQ( 28, match[ 0 ].length() );
}

TEST_F( TestTimeUtil, WillConvertValidRFC3339ToMillis )
{
    auto millis_orig = TimeUtil::Now_MilliTimeStamp();
    std::string tstr = TimeUtil::MillisToRFC3339( millis_orig );
    auto millis_from_rfc = TimeUtil::RFC3339ToMillis( tstr );

    std::cout << __FUNCTION__ << ": orig:" << millis_orig << " => rfc:" << tstr << " => conv:" << millis_from_rfc << std::endl;

    ASSERT_EQ( millis_orig, millis_from_rfc );
}

TEST_F( TestTimeUtil, RFC3339WillReflectTheExactSecondsSubtracted )
{
    auto ts1 = 1628834666310;
    std::string tstr1 = TimeUtil::MillisToRFC3339( ts1 );

    auto ts_less_20_sec = ts1 - 20 * 1000;
    std::string tstr_less_20_sec = TimeUtil::MillisToRFC3339( ts_less_20_sec );

    EXPECT_TRUE( tstr1.find( ":26.31" ) != std::string::npos );
    EXPECT_TRUE( tstr_less_20_sec.find( ":06.31" ) != std::string::npos );
}

TEST_F( TestTimeUtil, DISABLED_RFC3339WillDecodePartialHourTimezones )
{
    auto newfoundland_time = TimeUtil::RFC3339ToMillis( "2021-08-13T06:04:26.18-02:30" );
    auto chatam_time = TimeUtil::RFC3339ToMillis( "2021-08-13T06:04:26.05+12:45" );

    EXPECT_EQ( 1628850866180, newfoundland_time );
    EXPECT_EQ( 1628905766050, chatam_time );
}

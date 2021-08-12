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
    ASSERT_TRUE( match[0].matched );
    ASSERT_EQ( 28, match[ 0 ].length() );
}

TEST_F( TestTimeUtil, WillConvertMillisToValidRFC3339 )
{
    auto millis = TimeUtil::Now_HundredMilliTimeStamp();
    std::string tstr = TimeUtil::MilliTsToRFC3339( millis );

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
    auto millis_orig = TimeUtil::Now_HundredMilliTimeStamp();
    std::string tstr = TimeUtil::MilliTsToRFC3339( millis_orig );
    auto millis_from_rfc = TimeUtil::RFC3339ToMilliTs( tstr );

    std::cout << __FUNCTION__ << ": orig:" << millis_orig << " => rfc:" << tstr << " => conv:" << millis_from_rfc << std::endl;

    ASSERT_EQ( millis_orig, millis_from_rfc );
}

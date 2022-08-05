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

TEST_F( TestTimeUtil, WillGenerateLocalRFC3339TimeString )
{
    std::string tstr = TimeUtil::Now_LocalRFC3339();

    std::cout << __FUNCTION__ << ": " << tstr << std::endl;

    std::regex rexp( RFC3339_REGEXP );
    std::smatch match;
    std::regex_search( tstr, match, rexp );

    ASSERT_TRUE( match.ready() );
    ASSERT_EQ( 2, match.size() );
    ASSERT_TRUE( match[ 0 ].matched );
    ASSERT_EQ( 28, match[ 0 ].length() );
}

TEST_F( TestTimeUtil, WillGenerateZuluRFC3339TimeString )
{
    std::string tstr = TimeUtil::Now_ZuluRFC3339();

    std::cout << __FUNCTION__ << ": " << tstr << std::endl;

    std::regex rexp( ZULU_RFC3339_REGEXP );
    std::smatch match;
    std::regex_search( tstr, match, rexp );

    ASSERT_TRUE( match.ready() );
    ASSERT_EQ( 2, match.size() );
    ASSERT_TRUE( match[ 0 ].matched );
    ASSERT_EQ( 23, match[ 0 ].length() );
}

TEST_F( TestTimeUtil, WillConvertMillisToLocalRFC3339TimeString )
{
    auto millis = TimeUtil::Now_MilliTimeStamp();
    std::string tstr = TimeUtil::MillisToLocalRFC3339( millis );

    std::cout << __FUNCTION__ << ": " << tstr << std::endl;

    std::regex rexp( RFC3339_REGEXP );
    std::smatch match;
    std::regex_search( tstr, match, rexp );

    ASSERT_TRUE( match.ready() );
    ASSERT_EQ( 2, match.size() );
    ASSERT_TRUE( match[ 0 ].matched );
    ASSERT_EQ( 28, match[ 0 ].length() );
}

TEST_F( TestTimeUtil, WillConvertMillisToZuluRFC3339TimeString )
{
    auto millis = TimeUtil::Now_MilliTimeStamp();
    std::string tstr = TimeUtil::MillisToZuluRFC3339( millis );

    std::cout << __FUNCTION__ << ": " << tstr << std::endl;

    std::regex rexp( ZULU_RFC3339_REGEXP );
    std::smatch match;
    std::regex_search( tstr, match, rexp );

    ASSERT_TRUE( match.ready() );
    ASSERT_EQ( 2, match.size() );
    ASSERT_TRUE( match[ 0 ].matched );
    ASSERT_EQ( 23, match[ 0 ].length() );
}

TEST_F( TestTimeUtil, WillConvertGeneratedZuluRFC3339TimeStringBackToMillis )
{
    auto millis_orig = TimeUtil::Now_MilliTimeStamp();
    std::string tstr = TimeUtil::MillisToZuluRFC3339( millis_orig );
    auto millis_from_rfc = TimeUtil::RFC3339ToMillis( tstr );

    std::cout << __FUNCTION__ << ": orig:" << millis_orig << " => rfc:" << tstr << " => conv:" << millis_from_rfc << std::endl;

    ASSERT_EQ( millis_orig, millis_from_rfc );
}

TEST_F( TestTimeUtil, WillConvertLocalRFC3339TimeStringToMillis )
{
    auto millis_orig = TimeUtil::Now_MilliTimeStamp();
    std::string tstr = TimeUtil::MillisToLocalRFC3339( millis_orig );
    auto millis_from_rfc = TimeUtil::RFC3339ToMillis( tstr );

    std::cout << __FUNCTION__ << ": orig:" << millis_orig << " => rfc:" << tstr << " => conv:" << millis_from_rfc << std::endl;

    ASSERT_EQ( millis_orig, millis_from_rfc );
}

TEST_F( TestTimeUtil, WillConvertYymmddHHMMSSWithoutTimeZoneToMillis )
{
    std::string tstr = "2021-08-11T06:05:54.80";
    auto millis_from_rfc = TimeUtil::RFC3339ToMillis( tstr );

    std::cout << __FUNCTION__ << ": rfc:" << tstr << " => conv:" << millis_from_rfc << std::endl;

    ASSERT_EQ( 1628661954800, millis_from_rfc );
}

TEST_F( TestTimeUtil, WillConvertYymmddHHMMSSWithoutMillisAndTimeZoneToMillis )
{
    std::string tstr = "2021-08-11T06:05:54";
    auto millis_from_rfc = TimeUtil::RFC3339ToMillis( tstr );

    std::cout << __FUNCTION__ << ": rfc:" << tstr << " => conv:" << millis_from_rfc << std::endl;

    ASSERT_EQ( 1628661954000, millis_from_rfc );
}

TEST_F( TestTimeUtil, WillSafelyHandleConversionOfBadTimeStringToMillis )
{
    std::string tstr = "b4b3jn5n64n3n3n2";
    auto millis_from_rfc = TimeUtil::RFC3339ToMillis( tstr );

    std::cout << __FUNCTION__ << ": rfc:" << tstr << " => conv:" << millis_from_rfc << std::endl;

    ASSERT_EQ( 0, millis_from_rfc );
}

TEST_F( TestTimeUtil, WillSafelyHandleConversionOfBadDateStringToMillis )
{
    std::string tstr = "2021-p8-11T06:05:54";
    auto millis_from_rfc = TimeUtil::RFC3339ToMillis( tstr );

    std::cout << __FUNCTION__ << ": rfc:" << tstr << " => conv:" << millis_from_rfc << std::endl;

    ASSERT_EQ( 0, millis_from_rfc );
}

TEST_F( TestTimeUtil, WillSafelyHandleConversionOfMalformedTimeToMillis )
{
    std::string tstr = "2021-08-11Tp6:05:54";
    auto millis_from_rfc = TimeUtil::RFC3339ToMillis( tstr );

    std::cout << __FUNCTION__ << ": rfc:" << tstr << " => conv:" << millis_from_rfc << std::endl;

    ASSERT_EQ( 0, millis_from_rfc );
}

TEST_F( TestTimeUtil, WillSafelyHandleConversionOfMalformedMillisecondsToMillis )
{
    std::string tstr = "2021-08-11T06:05:54.p2-07:00";
    auto millis_from_rfc = TimeUtil::RFC3339ToMillis( tstr );

    std::cout << __FUNCTION__ << ": rfc:" << tstr << " => conv:" << millis_from_rfc << std::endl;

    ASSERT_EQ( 1628687154000, millis_from_rfc );
}

TEST_F( TestTimeUtil, WillSafelyHandleConversionOfMalformedTimeZoneToMillis )
{
    std::string tstr = "2021-08-11T06:05:54.02-p7:00";
    auto millis_from_rfc = TimeUtil::RFC3339ToMillis( tstr );

    std::cout << __FUNCTION__ << ": rfc:" << tstr << " => conv:" << millis_from_rfc << std::endl;

    ASSERT_EQ( 1628661954020, millis_from_rfc );
}

TEST_F( TestTimeUtil, WillConvertAcceptableUtcRFC3339TimeZoneFormatsToMillis )
{
    std::string tstr1 = "2021-08-11T06:05:54.02Z"; //Zulu timezone RFC3339 format
    std::string tstr2 = "2021-08-11T06:05:54.02+00:00"; //Standard UTC timezone RFC3339 format
    std::string tstr3 = "2021-08-11T06:05:54.02-00:00"; //Standard UTC timezone RFC3339 format
    auto millis_from_rfc1 = TimeUtil::RFC3339ToMillis( tstr1 );
    auto millis_from_rfc2 = TimeUtil::RFC3339ToMillis( tstr2 );
    auto millis_from_rfc3 = TimeUtil::RFC3339ToMillis( tstr3 );

    std::cout << __FUNCTION__ << ": rfc:" << tstr1 << " => conv:" << millis_from_rfc1 << std::endl;
    std::cout << __FUNCTION__ << ": rfc:" << tstr2 << " => conv:" << millis_from_rfc2 << std::endl;
    std::cout << __FUNCTION__ << ": rfc:" << tstr3 << " => conv:" << millis_from_rfc3 << std::endl;

    ASSERT_EQ( 1628661954020, millis_from_rfc1 );
    ASSERT_EQ( 1628661954020, millis_from_rfc2 );
    ASSERT_EQ( 1628661954020, millis_from_rfc3 );
}

TEST_F( TestTimeUtil, RFC3339WillReflectTheExactSecondsSubtracted )
{
    auto ts1 = 1628834666310;
    std::string tstr1 = TimeUtil::MillisToLocalRFC3339( ts1 );

    auto ts_less_20_sec = ts1 - 20 * 1000;
    std::string tstr_less_20_sec = TimeUtil::MillisToLocalRFC3339( ts_less_20_sec );

    EXPECT_TRUE( tstr1.find( ":26.31" ) != std::string::npos );
    EXPECT_TRUE( tstr_less_20_sec.find( ":06.31" ) != std::string::npos );
}

TEST_F( TestTimeUtil, RFC3339WillDecodePartialHourTimezones )
{
    auto newfoundland_time = TimeUtil::RFC3339ToMillis( "2021-08-13T06:04:26.18-02:30" );
    auto chatam_time = TimeUtil::RFC3339ToMillis( "2021-08-13T06:04:26.05+12:45" );

    EXPECT_EQ( 1628843666180, newfoundland_time );
    EXPECT_EQ( 1628788766050, chatam_time );
}

#include "TimeUtil.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

//https://stackoverflow.com/questions/54325137/c-rfc3339-timestamp-with-milliseconds-using-stdchrono
std::string TimeUtil::Now_RFC3339()
{
    const auto now_ms = std::chrono::time_point_cast< std::chrono::milliseconds >( std::chrono::system_clock::now() );
    const auto now_s = std::chrono::time_point_cast< std::chrono::seconds >( now_ms );
    const auto millis = now_ms - now_s;
    const auto c_now = std::chrono::system_clock::to_time_t( now_s );

    std::stringstream ss;
    struct tm tmbuf;
    gmtime_s( &tmbuf, &c_now );
    ss << std::put_time( &tmbuf, "%FT%T" )
        << '.'
        << std::setfill( '0' ) << std::setw( 2 ) << millis.count() / 100
        << std::put_time( &tmbuf, "%z" );

    std::string result = ss.str();
    result.insert( result.end() - 2, ':' );

    return result;
}


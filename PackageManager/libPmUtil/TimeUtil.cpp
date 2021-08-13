#include "TimeUtil.h"
#include <time.h>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <regex>
#include "StringUtil.h"

using namespace std::chrono;
using namespace std;

/* https://stackoverflow.com/questions/54325137/c-rfc3339-timestamp-with-milliseconds-using-stdchrono
   generated string formats:
   2021-08-11T06:05:54.02-07:00
   2021-08-11T19:27:19.01+00:00
*/
std::string TimeUtil::Now_RFC3339()
{
    const auto now_ms = time_point_cast< milliseconds >( system_clock::now() );
    const auto now_s = time_point_cast< seconds >( now_ms );
    const auto millis = now_ms - now_s;
    const auto c_now = system_clock::to_time_t( now_s );

    stringstream ss;
    struct tm tmbuf;
    gmtime_s( &tmbuf, &c_now );
    ss << put_time( &tmbuf, "%FT%T" )
        << '.'
        << setfill( '0' ) << setw( 2 ) << millis.count() / 100
        << put_time( &tmbuf, "%z" );

    std::string result = ss.str();
    result.insert( result.end() - 2, ':' );

    return result;
}

__time64_t TimeUtil::Now_MilliTimeStamp()
{
    const auto tp_now_millis = time_point_cast< milliseconds >( system_clock::now() );
    auto millis_dur = duration_cast< milliseconds >( tp_now_millis.time_since_epoch() ).count();

    //RFC3339 only retains hundreds of milliseconds
    millis_dur /= 100;
    millis_dur *= 100;

    return millis_dur;
}

std::string TimeUtil::MillisToRFC3339( __time64_t milliTimeStamp )
{
    const auto millis_dur = duration_cast< milliseconds >( milliseconds { milliTimeStamp } );
    const auto tp_ms = time_point<system_clock, milliseconds> { millis_dur };
    const auto tp_s = time_point_cast< seconds >( tp_ms );
    const auto millis = tp_ms - tp_s;
    const auto c_now = system_clock::to_time_t( tp_s );

    stringstream ss;
    struct tm tmbuf;
    gmtime_s( &tmbuf, &c_now );
    ss << put_time( &tmbuf, "%FT%T" )
        << '.'
        << setfill( '0' ) << setw( 2 ) << millis.count() / 100
        << put_time( &tmbuf, "%z" );

    std::string result = ss.str();
    result.insert( result.end() - 2, ':' );

    return result;
}

/*  https://stackoverflow.com/questions/321849/strptime-equivalent-on-windows  */
#ifdef _WIN32
extern "C" char* strptime( const char* s, const char* f, struct tm* tm )
{
    std::istringstream input( s );
    input.imbue( std::locale( setlocale( LC_ALL, nullptr ) ) );
    input >> std::get_time( tm, f );
    if( input.fail() ) {
        return nullptr;
    }

    return ( char* )( s + ( int )input.tellg() );
}
#endif

__time64_t TimeUtil::RFC3339ToMillis( const std::string& rfc3339_time )
{
    int msec = 0, tzsign = 1;
    __time64_t timestamp = 0;

    struct tm tm = { 0 };
    auto parts = StringUtil::Split( rfc3339_time, '.' );
    std::string dttm = parts[ 0 ]; //2021-08-11T06:05:54
    std::string mstz = parts[ 1 ]; //02-07:00

    if( strptime( dttm.c_str(), "%Y-%m-%dT%H:%M:%S", &tm ) ) {
        timestamp = mktime( &tm );
    }

    std::stringstream st;
    std::string tzstr = "";
    if( mstz.find( '-' ) != std::string::npos ) {
        parts = StringUtil::Split( mstz, '-' );
        st.clear(); st << parts[ 0 ]; st >> msec;
        tzstr = parts[ 1 ];
        tzsign = -1;
    }
    else if( mstz.find( '+' ) != std::string::npos ) {
        parts = StringUtil::Split( mstz, '+' );
        st.clear(); st << parts[ 0 ]; st >> msec;
        tzstr = parts[ 1 ];
    }
    else if( mstz.size() >= 2 )
    {
        st.clear(); st << mstz.substr( 0, 2 ); st >> msec;
    }

    if( tzstr.size() == 5 )
    {
        tm = { 0 };
        if( strptime( tzstr.c_str(), "%H:%M", &tm ) ) {
            int tzh = tm.tm_hour, tzm = tm.tm_min;
            __time64_t tzofs = ( __time64_t )( tzsign * ( tzh * 3600 + tzm * 60 ) );
            timestamp += tzofs;
        }
    }

    auto millis_dur = duration_cast< milliseconds >( seconds { timestamp } ).count() + ( __time64_t )msec * 100;
    return millis_dur;
}

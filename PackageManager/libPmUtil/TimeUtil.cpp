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
std::string TimeUtil::Now_LocalRFC3339()
{
    const auto now_ms = time_point_cast< milliseconds >( system_clock::now() );
    const auto now_s = time_point_cast< seconds >( now_ms );
    const auto millis = now_ms - now_s;
    const auto c_now = system_clock::to_time_t( now_s );

    stringstream ss;
    struct tm tmbuf;
    localtime_s( &tmbuf, &c_now );
    ss << put_time( &tmbuf, "%FT%T" )
        << '.'
        << setfill( '0' ) << setw( 2 ) << millis.count() / 10
        << put_time( &tmbuf, "%z" );

    std::string result = ss.str();
    result.insert( result.end() - 2, ':' );

    return result;
}

/* https://stackoverflow.com/questions/54325137/c-rfc3339-timestamp-with-milliseconds-using-stdchrono
   https://medium.com/easyread/understanding-about-rfc-3339-for-datetime-formatting-in-software-engineering-940aa5d5f68a
   https://stackoverflow.com/questions/9706688/what-does-the-z-mean-in-unix-timestamp-120314170138z/9706777#9706777
   generated UTC (Zulu) string format:
   2022-07-11T16:06:51.96Z
*/
std::string TimeUtil::Now_ZuluRFC3339()
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
        << setfill( '0' ) << setw( 2 ) << millis.count() / 10
        << "Z";

    std::string result = ss.str();

    return result;
}

__time64_t TimeUtil::Now_MilliTimeStamp()
{
    const auto tp_now_millis = time_point_cast< milliseconds >( system_clock::now() );
    auto millis_dur = duration_cast< milliseconds >( tp_now_millis.time_since_epoch() ).count();

    //remove units - RFC3339 only encodes hundreds + tens of milliseconds
    //2021-08-11T06:05:54.02-07:00
    millis_dur /= 10;
    millis_dur *= 10;

    return millis_dur;
}

std::string TimeUtil::MillisToLocalRFC3339( __time64_t milliTimeStamp )
{
    const auto millis_dur = duration_cast< milliseconds >( milliseconds{ milliTimeStamp } );
    const auto tp_ms = time_point<system_clock, milliseconds>{ millis_dur };
    const auto tp_s = time_point_cast< seconds >( tp_ms );
    const auto millis = tp_ms - tp_s;
    const auto c_now = system_clock::to_time_t( tp_s );

    stringstream ss;
    struct tm tmbuf;
    localtime_s( &tmbuf, &c_now );
    ss << put_time( &tmbuf, "%FT%T" )
        << '.'
        << setfill( '0' ) << setw( 2 ) << millis.count() / 10
        << put_time( &tmbuf, "%z" );

    std::string result = ss.str();
    result.insert( result.end() - 2, ':' );

    return result;
}

std::string TimeUtil::MillisToZuluRFC3339( __time64_t milliTimeStamp )
{
    const auto millis_dur = duration_cast< milliseconds >( milliseconds{ milliTimeStamp } );
    const auto tp_ms = time_point<system_clock, milliseconds>{ millis_dur };
    const auto tp_s = time_point_cast< seconds >( tp_ms );
    const auto millis = tp_ms - tp_s;
    const auto c_now = system_clock::to_time_t( tp_s );

    stringstream ss;
    struct tm tmbuf;
    gmtime_s( &tmbuf, &c_now );
    ss << put_time( &tmbuf, "%FT%T" )
        << '.'
        << setfill( '0' ) << setw( 2 ) << millis.count() / 10
        << "Z";

    std::string result = ss.str();

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

/* struct tm to seconds since Unix epoch
   https://stackoverflow.com/questions/530519/stdmktime-and-timezone-info
   http://www.catb.org/esr/time-programming/
*/
time_t my_timegm( struct tm* t )
{
    long year;
    time_t result;
#define MONTHSPERYEAR   12
    static const int cumdays[ MONTHSPERYEAR ] =
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

    year = 1900 + t->tm_year + t->tm_mon / MONTHSPERYEAR;
    result = ( ( long long )year - 1970 ) * 365 + cumdays[ t->tm_mon % MONTHSPERYEAR ];
    result += ( year - 1968 ) / 4;
    result -= ( year - 1900 ) / 100;
    result += ( year - 1600 ) / 400;
    if( ( year % 4 ) == 0 && ( ( year % 100 ) != 0 || ( year % 400 ) == 0 ) &&
        ( t->tm_mon % MONTHSPERYEAR ) < 2 )
        result--;
    result += ( long long )t->tm_mday - 1;
    result *= 24;
    result += t->tm_hour;
    result *= 60;
    result += t->tm_min;
    result *= 60;
    result += t->tm_sec;
    if( t->tm_isdst == 1 )
        result -= 3600;
    return ( result );
}

/* converts either long RFC3339 formats or short UTC time formats into millisecond timestamps
   up to tens of milliseconds precision */
__time64_t TimeUtil::RFC3339ToMillis( const std::string& rfc3339_time )
{
    int msec = 0, tzsign = 1;
    __time64_t timestamp = 0;

    struct tm tm = { 0 };
    auto parts = StringUtil::Split( rfc3339_time, '.' );

    std::string dttm = "";
    if( parts.size() >= 1 ) dttm = parts[ 0 ]; //2021-08-11T06:05:54
    std::string mstz = "";
    if( parts.size() >= 2 ) mstz = parts[ 1 ]; //02-07:00

    if( strptime( dttm.c_str(), "%Y-%m-%dT%H:%M:%S", &tm ) != nullptr ) {
        timestamp = my_timegm( &tm );
    }

    std::stringstream st;
    std::string tzstr = "";
    if( mstz.find( '-' ) != std::string::npos ) {
        parts = StringUtil::Split( mstz, '-' );
        st.clear(); st << parts[ 0 ]; st >> msec;
        tzstr = parts[ 1 ];
        tzsign = 1; //add back the subtracted timezone offset to get GMT
    }
    else if( mstz.find( '+' ) != std::string::npos ) {
        parts = StringUtil::Split( mstz, '+' );
        st.clear(); st << parts[ 0 ]; st >> msec;
        tzstr = parts[ 1 ];
        tzsign = -1; //subtract back the added timezone offset to get GMT
    }
    else if( mstz.size() >= 2 )
    {
        //if timezone is not specified as +/- offset then just extract the tens of millisec
        //and assume 'Z' (Zulu/GMT/UTC) timezone as that is the only other acceptable RFC3339 format
        st.clear(); st << mstz.substr( 0, 2 ); st >> msec;
    }

    if( tzstr.size() == 5 )
    {
        tm = { 0 };
        if( strptime( tzstr.c_str(), "%H:%M", &tm ) != nullptr ) {
            int tzh = tm.tm_hour, tzm = tm.tm_min;
            __time64_t tzofs = ( __time64_t )( ( long long )tzsign * ( ( long long )tzh * 3600 + ( long long )tzm * 60 ) );
            timestamp += tzofs;
        }
    }

    auto millis_dur = duration_cast< milliseconds >( seconds{ timestamp } ).count()
        + ( __time64_t )msec * 10; //we get hundreds and tens of msec from rfc string, but no units
    return millis_dur;
}

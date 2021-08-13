#pragma once

#include <string>
#include <ctime>

#define RFC3339_REGEXP "^(\\d{4}-\\d{2}-\\d{2}T\\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{2}[-+]\\d{2}\\:\\d{2})"

class TimeUtil
{
public:
    TimeUtil() {}
    virtual ~TimeUtil() {}

    static std::string Now_RFC3339();
    static __time64_t Now_MilliTimeStamp();
    static std::string MillisToRFC3339( __time64_t milliTimeStamp );
    static __time64_t RFC3339ToMillis( const std::string& rfc3339_time );
};

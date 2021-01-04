#pragma once

#include <string>

class TimeUtil
{
public:
    TimeUtil() {}
    virtual ~TimeUtil() {}

    static std::string Now_RFC3339();
};

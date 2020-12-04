#pragma once

#include <string>

class StringUtil
{
public:
    static std::string LTrim( const std::string& s );
    static std::string RTrim( const std::string& s );
    static std::string Trim( const std::string& s );
};

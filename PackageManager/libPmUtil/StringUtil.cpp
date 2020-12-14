#include "StringUtil.h"

#include <iostream>
#include <algorithm>

const std::string WHITESPACE = " \n\r\t\f\v\0";

std::string StringUtil::LTrim( const std::string& s )
{
    size_t start = s.find_first_not_of( WHITESPACE );
    return ( start == std::string::npos ) ? "" : s.substr( start );
}

std::string StringUtil::RTrim( const std::string& s )
{
    size_t end = s.find_last_not_of( WHITESPACE );
    return ( end == std::string::npos ) ? "" : s.substr( 0, end + 1 );
}

std::string StringUtil::Trim( const std::string& s )
{
    return RTrim( LTrim( s ) );
}

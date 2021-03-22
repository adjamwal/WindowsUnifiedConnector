#include "StringUtil.h"

#include <iostream>
#include <algorithm>
#include <sstream>

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

void StringUtil::ReplaceStringInPlace( std::string& subject, const std::string& search, const std::string& replace )
{
    size_t pos = 0;
    while( ( pos = subject.find( search, pos ) ) != std::string::npos ) {
        subject.replace( pos, search.length(), replace );
        pos += replace.length();
    }
}

std::vector<std::string> StringUtil::Split( const std::string source, const char separator )
{
    std::vector<std::string> parts;
    std::istringstream stream( source );
    std::string s;
    while( getline( stream, s, separator ) ) {
        parts.push_back( s );
    }

    return parts;
}

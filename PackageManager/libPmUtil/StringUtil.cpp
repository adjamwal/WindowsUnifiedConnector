#include "StringUtil.h"

#include <iostream>
#include <algorithm>
#include <sstream>

const std::string WHITESPACE = " \n\r\t\f\v\0";
const std::wstring L_WHITESPACE = L" \n\r\t\f\v\0";

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

std::wstring StringUtil::LTrim( const std::wstring& s )
{
    size_t start = s.find_first_not_of( L_WHITESPACE );
    return ( start == std::wstring::npos ) ? L"" : s.substr( start );
}

std::wstring StringUtil::RTrim( const std::wstring& s )
{
    size_t end = s.find_last_not_of( L_WHITESPACE );
    return ( end == std::wstring::npos ) ? L"" : s.substr( 0, end + 1 );
}

std::wstring StringUtil::Trim( const std::wstring& s )
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

std::wstring StringUtil::Str2WStr( const std::string& str )
{
    auto ws = std::make_unique<wchar_t[]>( str.size() + 1 );
    mbstowcs_s( nullptr, ws.get(), str.size() + 1, str.c_str(), str.size() );
    return ws.get();
}

std::string StringUtil::WStr2Str( const std::wstring& wstr )
{
    std::string str( wstr.length(), 0 );
    std::transform( wstr.begin(), wstr.end(), str.begin(), []( wchar_t c ) { return ( char )c; } );
    return str;
}

bool StringUtil::EqualsIgnoreCase( const std::string& a, const std::string& b )
{
    return std::equal( a.begin(), a.end(),
        b.begin(), b.end(),
        []( char a, char b ) {
            return tolower( a ) == tolower( b );
        } );
}

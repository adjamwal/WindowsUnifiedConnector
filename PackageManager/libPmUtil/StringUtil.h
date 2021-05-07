#pragma once

#include <string>
#include <vector>

class StringUtil
{
public:
    static std::string LTrim( const std::string& s );
    static std::string RTrim( const std::string& s );
    static std::string Trim( const std::string& s );
    static void ReplaceStringInPlace( std::string& subject, const std::string& search, const std::string& replace );
    static std::vector<std::string> Split( const std::string source, const char separator );
    static std::wstring Str2WStr( const std::string& str );
    static bool NoCaseEquals( const std::string& a, const std::string& b );
};

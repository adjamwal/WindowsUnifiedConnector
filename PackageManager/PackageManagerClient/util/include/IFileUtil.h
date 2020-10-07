#pragma once

#include <string>

class IFileUtil
{
public:
    IFileUtil() {}
    virtual ~IFileUtil() {}

    virtual std::string ReadFile( const std::string& filename ) = 0;
};
#pragma once

#include <string>

class IPmBootstrap
{
public:
    IPmBootstrap() {}
    virtual  ~IPmBootstrap() {}

    virtual int32_t LoadPmBootstrap( const std::string& filename ) = 0;
    virtual std::string GetIdentifyUri() = 0;
};

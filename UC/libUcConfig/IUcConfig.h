#pragma once

#include <stdint.h>
#include <string>

class IUcConfig
{
public:
    IUcConfig() {}
    virtual ~IUcConfig() {}

    virtual bool LoadConfig() = 0;
    virtual bool VerifyConfig( const std::wstring& path = L"" ) = 0;

    virtual uint32_t GetLogLevel() = 0;
};
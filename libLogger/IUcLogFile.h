#pragma once

#include <cstdlib>
#include <cstdint>

class IUcLogFile
{
public:
    IUcLogFile() {};
    virtual ~IUcLogFile() {};

    virtual void Init( const wchar_t* logname = NULL ) = 0;
    virtual void WriteLogLine( const char* logLevel, const char* logLine ) = 0;
    virtual void WriteLogLine( const wchar_t* logLevel, const wchar_t* logLine ) = 0;
    virtual void SetLogConfig( uint32_t fileSize, uint32_t logFiles ) = 0;
};

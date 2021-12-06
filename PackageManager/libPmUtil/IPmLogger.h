#pragma once

#include <cstdarg>

class IPMLogger
{
public:
    enum Severity {
        LOG_ALERT = 1,
        LOG_CRITICAL = 2,
        LOG_ERROR = 3,
        LOG_WARNING = 4,
        LOG_NOTICE = 5,
        LOG_INFO = 6,
        LOG_DEBUG = 7
    };

    virtual ~IPMLogger() {}
    virtual void Log( Severity severity, const char* msgFormatter, ... ) = 0;
    virtual void Log( Severity severity, const wchar_t* msgFormatter, ... ) = 0;
    virtual void Log( Severity severity, const char* msgFormatter, va_list args ) = 0;
    virtual void Log( Severity severity, const wchar_t* msgFormatter, va_list args ) = 0;

    virtual void SetLogLevel( Severity severity ) = 0;
};

IPMLogger* GetPMLogger();
void SetPMLogger( IPMLogger* logger );

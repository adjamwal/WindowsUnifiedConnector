#pragma once

#include <cstdarg>

class IPMLogger
{
public:
    enum Severity {
        LOG_EMERGENCY,
        LOG_ALERT,
        LOG_CRITICAL,
        LOG_ERROR,
        LOG_WARNING,
        LOG_NOTICE,
        LOG_INFO,
        LOG_DEBUG
    };

    virtual ~IPMLogger() {}
    virtual void Log( Severity serverity, const char* msgFormatter, ... ) = 0;
    virtual void Log( Severity serverity, const wchar_t* msgFormatter, ... ) = 0;
    virtual void Log( Severity serverity, const char* msgFormatter, va_list args ) = 0;
    virtual void Log( Severity serverity, const wchar_t* msgFormatter, va_list args ) = 0;

    virtual void SetLogLevel( Severity severity ) = 0;
};

IPMLogger* GetPMLogger();
void SetPMLogger( IPMLogger* logger );

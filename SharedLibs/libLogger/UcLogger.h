#pragma once

#pragma once
#include "IUcLogger.h"

class IUcLogFile;

class UcLogger : public IUcLogger
{
public:
    UcLogger( IUcLogFile& logFile );
    ~UcLogger();

    void Log( Severity serverity, const char* msgFormatter, ... ) override;
    void Log( Severity serverity, const wchar_t* msgFormatter, ... ) override;
    void Log( Severity serverity, const char* msgFormatter, va_list args ) override;
    void Log( Severity serverity, const wchar_t* msgFormatter, va_list args ) override;

    void SetLogLevel( Severity logLevel ) override;

private:
    Severity m_logLevel;
    IUcLogFile& m_logFile;

    void LogWithError( Severity level, const char* msgFormatter, va_list args );
    void LogWithError( Severity level, const wchar_t* msgFormatter, va_list args );
    const char* LogLevelStr( Severity level );
    const wchar_t* LogLevelStrW( Severity level );
};



#pragma once

#include "IUcLogger.h"

class MsiLogger : public IUcLogger
{
public:
    MsiLogger();
    ~MsiLogger();

    void Log( Severity serverity, const char* msgFormatter, ... ) override;
    void Log( Severity serverity, const wchar_t* msgFormatter, ... ) override;
    void Log( Severity serverity, const char* msgFormatter, va_list args ) override;
    void Log( Severity serverity, const wchar_t* msgFormatter, va_list args ) override;

    void SetLogLevel( Severity logLevel ) override;

private:
    Severity m_logLevel;

    void LogWithError( Severity level, const char* msgFormatter, va_list args );
    void LogWithError( Severity level, const wchar_t* msgFormatter, va_list args );
};



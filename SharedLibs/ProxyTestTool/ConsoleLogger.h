#pragma once

#include "IUcLogger.h"
#include <mutex>
#include <time.h>
#include <Windows.h>

class ConsoleLogger : public IUcLogger
{
public:
    ConsoleLogger() {}
    ~ConsoleLogger() {}

    virtual void Log( Severity severity, const char* msgFormatter, ... ) override;
    virtual void Log( Severity severity, const wchar_t* msgFormatter, ... ) override;
    virtual void Log( Severity severity, const char* msgFormatter, va_list args ) override;
    virtual void Log( Severity severity, const wchar_t* msgFormatter, va_list args ) override;

    virtual void SetLogLevel( Severity severity ) override;
private:
    std::mutex m_logMutex;
};


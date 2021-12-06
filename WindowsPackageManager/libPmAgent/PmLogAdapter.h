#pragma once

#include "IPmLogger.h"

class PmLogAdapter : public IPMLogger
{
public:
    PmLogAdapter();
    ~PmLogAdapter();

    void Log( Severity severity, const char* msgFormatter, ... ) override;
    void Log( Severity severity, const wchar_t* msgFormatter, ... ) override;
    void Log( Severity severity, const char* msgFormatter, va_list args ) override;
    void Log( Severity severity, const wchar_t* msgFormatter, va_list args ) override;

    void SetLogLevel( Severity severity ) override;
};

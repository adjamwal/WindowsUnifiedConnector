#pragma once

#include "IPmLogger.h"

class PmLogAdapter : public IPMLogger
{
public:
    PmLogAdapter();
    ~PmLogAdapter();

    void Log( Severity serverity, const char* msgFormatter, ... ) override;
    void Log( Severity serverity, const wchar_t* msgFormatter, ... ) override;
    void Log( Severity serverity, const char* msgFormatter, va_list args ) override;
    void Log( Severity serverity, const wchar_t* msgFormatter, va_list args ) override;

    void SetLogLevel( Severity severity ) override;
};

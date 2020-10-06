#include "pch.h"

#include "PmLogAdapter.h"
#include "IUcLogger.h"

PmLogAdapter::PmLogAdapter()
{

}

PmLogAdapter::~PmLogAdapter()
{

}

void PmLogAdapter::Log( Severity serverity, const char* msgFormatter, ... )
{
    va_list args;
    va_start( args, msgFormatter );
    GetUcLogger()->Log( ( IUcLogger::Severity )serverity, msgFormatter, args );
    va_end( args );
}

void PmLogAdapter::Log( Severity serverity, const wchar_t* msgFormatter, ... )
{
    va_list args;
    va_start( args, msgFormatter );
    GetUcLogger()->Log( ( IUcLogger::Severity )serverity, msgFormatter, args );
    va_end( args );
}

void PmLogAdapter::Log( Severity serverity, const char* msgFormatter, va_list args )
{
    GetUcLogger()->Log( ( IUcLogger::Severity )serverity, msgFormatter, args );
}

void PmLogAdapter::Log( Severity serverity, const wchar_t* msgFormatter, va_list args )
{
    GetUcLogger()->Log( ( IUcLogger::Severity )serverity, msgFormatter, args );
}
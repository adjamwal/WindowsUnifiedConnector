#include "pch.h"

#include "PmLogAdapter.h"
#include "IUcLogger.h"

PmLogAdapter::PmLogAdapter()
{

}

PmLogAdapter::~PmLogAdapter()
{

}

void PmLogAdapter::Log( Severity severity, const char* msgFormatter, ... )
{
    va_list args;
    va_start( args, msgFormatter );
    GetUcLogger()->Log( ( IUcLogger::Severity )severity, msgFormatter, args );
    va_end( args );
}

void PmLogAdapter::Log( Severity severity, const wchar_t* msgFormatter, ... )
{
    va_list args;
    va_start( args, msgFormatter );
    GetUcLogger()->Log( ( IUcLogger::Severity )severity, msgFormatter, args );
    va_end( args );
}

void PmLogAdapter::Log( Severity severity, const char* msgFormatter, va_list args )
{
    GetUcLogger()->Log( ( IUcLogger::Severity )severity, msgFormatter, args );
}

void PmLogAdapter::Log( Severity severity, const wchar_t* msgFormatter, va_list args )
{
    GetUcLogger()->Log( ( IUcLogger::Severity )severity, msgFormatter, args );
}

void PmLogAdapter::SetLogLevel( Severity severity )
{
    GetUcLogger()->SetLogLevel( (IUcLogger::Severity)severity );
}
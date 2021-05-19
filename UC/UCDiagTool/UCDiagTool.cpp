#include "pch.h"
#include "UcLogFile.h"
#include "UcLogger.h"
#include "DiagToolContainer.h"
#include "IDiagTool.h"
#include "WindowsUtilities.h"

int main()
{
    std::wstring dataDir = WindowsUtilities::GetLogDir();

    UcLogFile logFile;
    logFile.Init();

    UcLogger logger( logFile );
    logger.SetLogLevel( IUcLogger::LOG_DEBUG );
    SetUcLogger( &logger );

    WLOG_DEBUG( L"Enter" );

    DiagToolContainer diagToolContainer;

    diagToolContainer.GetDiagTool().CreateDiagnosticPackage();

    WLOG_DEBUG( L"Exit" );
    return 0;
}

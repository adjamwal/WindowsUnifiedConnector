#pragma once

#if UC_CONSUMER == 1
#define SERVICE_NAME             L"ImmunetUnifiedConnector"
#define SERVICE_DISPLAY_NAME     L"Immunet Unified Connector"
#else
#define SERVICE_NAME             L"CiscoUnifiedConnector"
#define SERVICE_DISPLAY_NAME     L"Cisco Unified Connector"
#endif
#define SERVICE_START_TYPE       SERVICE_DEMAND_START 
#define SERVICE_DEPENDENCIES     L"" 
#define SERVICE_ACCOUNT          L"NT AUTHORITY\\System" 
#define SERVICE_PASSWORD         NULL 

#include "targetver.h"

#include <windows.h>
#include <winsvc.h>
#include <stdio.h>
#include <tchar.h>
#include <evntprov.h>

#define NOMINMAX    // disable min and max macros in windows.h

extern bool ShutdownRequested;

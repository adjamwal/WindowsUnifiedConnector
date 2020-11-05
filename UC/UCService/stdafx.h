#pragma once

#define SERVICE_NAME             L"UCService" 
#define SERVICE_DISPLAY_NAME     L"Windows Unified Connector Service" 
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

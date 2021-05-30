#pragma once

#include "CaSupportExports.h"
#include <string>

class IUcLogger;

extern "C" CA_SUPPORT_API void TestLogger( IUcLogger* logger );
extern "C" CA_SUPPORT_API bool GetUcidAndToken( IUcLogger * logger, std::string & ucid, std::string & ucidToken );
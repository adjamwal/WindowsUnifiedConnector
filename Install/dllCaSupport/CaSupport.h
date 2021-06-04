#pragma once

#include "CaSupportExports.h"
#include <string>

class IUcLogger;

extern "C" CA_SUPPORT_API void TestLogger( IUcLogger* logger );
extern "C" CA_SUPPORT_API bool CollectUCData( IUcLogger * logger, std::string & url, std::string & ucid, std::string & ucidToken );
extern "C" CA_SUPPORT_API bool SendEventOnUninstallBegin( IUcLogger* logger, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken );
extern "C" CA_SUPPORT_API bool SendEventOnUninstallError( IUcLogger* logger, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken );
extern "C" CA_SUPPORT_API bool SendEventOnUninstallComplete( IUcLogger* logger, std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken );

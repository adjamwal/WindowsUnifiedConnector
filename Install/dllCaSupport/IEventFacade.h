#pragma once

#include "IEventFacade.h"
#include <string>

class IEventFacade
{
public:
    virtual bool SendEventOnUninstallBegin( std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken ) = 0;
    virtual bool SendEventOnUninstallError( std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken ) = 0;
    virtual bool SendEventOnUninstallComplete( std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken ) = 0;
};

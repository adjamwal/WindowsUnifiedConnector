#pragma once

#include "IUcidFacade.h"
#include <string>

class IUcidFacade
{
public:
    virtual bool FetchCredentials( std::string& ucid, std::string& ucidToken ) = 0;
};

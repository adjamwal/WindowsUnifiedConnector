#pragma once

#include "IUcidFacade.h"
#include <string>

class IUcidFacade
{
public:
    virtual bool CollectUCData( std::string& url, std::string& ucid, std::string& ucidToken ) = 0;
};

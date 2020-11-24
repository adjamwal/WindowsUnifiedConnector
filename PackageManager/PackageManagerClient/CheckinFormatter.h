#pragma once

#include "ICheckinFormatter.h"

class CheckinFormatter : public ICheckinFormatter
{
public:
    CheckinFormatter();
    virtual ~CheckinFormatter();

    std::string GetJson( PackageInventory& inventory ) override;
};

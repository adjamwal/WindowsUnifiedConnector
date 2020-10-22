#pragma once

#include <string>

struct PackageInventory;

class ICheckinFormatter
{
public:
    ICheckinFormatter() {}
    virtual ~ICheckinFormatter() {}

    virtual std::string GetJson( PackageInventory& inventory ) = 0;
};

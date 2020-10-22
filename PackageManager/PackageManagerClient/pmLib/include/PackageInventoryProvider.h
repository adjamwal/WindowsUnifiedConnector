#pragma once

#include "IPackageInventoryProvider.h"
#include "PmTypes.h"

class PackageInventoryProvider : public IPackageInventoryProvider
{
public:
    PackageInventoryProvider();
    virtual ~PackageInventoryProvider();

    bool GetInventory( PackageInventory& inventory ) override;
};

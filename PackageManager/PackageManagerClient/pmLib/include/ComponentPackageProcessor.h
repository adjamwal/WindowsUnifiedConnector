#pragma once

#include "IComponentPackageProcessor.h"
#include "PmTypes.h"

class ComponentPackageProcessor : public IComponentPackageProcessor
{
public:
    ComponentPackageProcessor();
    virtual ~ComponentPackageProcessor();

    bool ProcessComponentPackage( PmComponent& componentPackage ) override;
};

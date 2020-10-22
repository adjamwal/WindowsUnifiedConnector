#pragma once

#include <string>

struct PmComponent;

class IComponentPackageProcessor
{
public:
    IComponentPackageProcessor() {}
    virtual ~IComponentPackageProcessor() {}

    virtual bool ProcessComponentPackage( PmComponent& componentPackage ) = 0;
};

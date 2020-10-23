#pragma once

#include "IComponentPackageProcessor.h"
#include "PmTypes.h"
#include <mutex>

class IPmCloud;

class ComponentPackageProcessor : public IComponentPackageProcessor
{
public:
    ComponentPackageProcessor( IPmCloud& pmCloud );
    virtual ~ComponentPackageProcessor();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool ProcessComponentPackage( PmComponent& componentPackage ) override;

private:
    std::mutex m_mutex;
    IPmCloud& m_pmCloud;
    IPmPlatformDependencies* m_dependencies;
    uint32_t m_fileCount;
};

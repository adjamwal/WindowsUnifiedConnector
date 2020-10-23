#pragma once

#include "IComponentPackageProcessor.h"
#include "PmTypes.h"
#include <mutex>

class IPmCloud;
class IFileUtil;

class ComponentPackageProcessor : public IComponentPackageProcessor
{
public:
    ComponentPackageProcessor( IPmCloud& pmCloud, IFileUtil& fileutil );
    virtual ~ComponentPackageProcessor();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool ProcessComponentPackage( PmComponent& componentPackage ) override;

private:
    std::mutex m_mutex;
    IPmCloud& m_pmCloud;
    IFileUtil& m_fileUtil;
    IPmPlatformDependencies* m_dependencies;
    uint32_t m_fileCount;
};

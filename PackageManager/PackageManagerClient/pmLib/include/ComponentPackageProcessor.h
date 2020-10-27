#pragma once

#include "IComponentPackageProcessor.h"
#include "PmTypes.h"
#include <mutex>

class IPmCloud;
class IFileUtil;
class ISslUtil;

class ComponentPackageProcessor : public IComponentPackageProcessor
{
public:
    ComponentPackageProcessor( IPmCloud& pmCloud, IFileUtil& fileutil, ISslUtil& sslUtil );
    virtual ~ComponentPackageProcessor();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool ProcessComponentPackage( PmComponent& componentPackage ) override;

private:
    std::mutex m_mutex;
    IPmCloud& m_pmCloud;
    IFileUtil& m_fileUtil;
    ISslUtil& m_sslUtil;
    IPmPlatformDependencies* m_dependencies;
    uint32_t m_fileCount;

    bool ProcessComponentPackageConfigs( PmComponent& componentPackage );
    bool ProcessComponentConfig( PackageConfigInfo& config );
};

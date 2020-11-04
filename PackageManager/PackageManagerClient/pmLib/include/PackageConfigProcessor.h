#pragma once

#include "IPackageConfigProcessor.h"
#include <mutex>

class IFileUtil;
class ISslUtil;

class PackageConfigProcessor : public IPackageConfigProcessor
{
public:
    PackageConfigProcessor( IFileUtil& fileUtil, ISslUtil& sslUtil );
    ~PackageConfigProcessor();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool ProcessConfig( PackageConfigInfo& config ) override;

private:
    bool AddConfig( PackageConfigInfo& config );
    bool RemoveConfig( PackageConfigInfo& config );

    IFileUtil& m_fileUtil;
    ISslUtil& m_sslUtil;
    std::mutex m_mutex;
    IPmPlatformDependencies* m_dependencies;
    uint32_t m_fileCount;
};
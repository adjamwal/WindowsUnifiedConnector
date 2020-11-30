#pragma once

#include "IComponentPackageProcessor.h"
#include "PmTypes.h"
#include <mutex>

class IPmCloud;
class IFileUtil;
class ISslUtil;
class IPackageConfigProcessor;
class ITokenAdapter;
class ICloudEventBuilder;
class ICloudEventPublisher;

class ComponentPackageProcessor : public IComponentPackageProcessor
{
public:
    ComponentPackageProcessor(
        IPmCloud& pmCloud, 
        IFileUtil& fileutil, 
        ISslUtil& sslUtil, 
        IPackageConfigProcessor& configProcessor, 
        ITokenAdapter& tokenAdapter,
        ICloudEventBuilder& eventBuilder,
        ICloudEventPublisher& eventPublisher
    );
    virtual ~ComponentPackageProcessor();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool ProcessComponentPackage( PmComponent& componentPackage ) override;

private:
    std::mutex m_mutex;
    IPmCloud& m_pmCloud;
    IFileUtil& m_fileUtil;
    ISslUtil& m_sslUtil;
    IPackageConfigProcessor& m_configProcessor;
    ITokenAdapter& m_tokenAdapter;
    ICloudEventBuilder& m_eventBuilder;
    ICloudEventPublisher& m_eventPublisher;
    IPmPlatformDependencies* m_dependencies;
    uint32_t m_fileCount;

    bool ProcessComponentPackageConfigs( PmComponent& componentPackage );
    bool ProcessComponentConfig( PackageConfigInfo& config );

    bool ExtractPackageNameAndVersion(const std::string& nameAndVersion, std::string& name, std::string& version);
};

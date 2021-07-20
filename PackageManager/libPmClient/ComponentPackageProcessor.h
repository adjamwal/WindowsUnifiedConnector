#pragma once

#include "IComponentPackageProcessor.h"
#include "PmTypes.h"
#include <mutex>

class IFileSysUtil;
class ISslUtil;
class IPackageConfigProcessor;
class IUcidAdapter;
class ICloudEventBuilder;
class ICloudEventPublisher;
class IUcUpgradeEventHandler;
class IInstallerCacheManager;

class ComponentPackageProcessor : public IComponentPackageProcessor
{
public:
    ComponentPackageProcessor(
        IInstallerCacheManager& installerManager,
        IFileSysUtil& fileutil,
        ISslUtil& sslUtil,
        IPackageConfigProcessor& configProcessor,
        IUcidAdapter& ucidAdapter,
        ICloudEventBuilder& eventBuilder,
        ICloudEventPublisher& eventPublisher,
        IUcUpgradeEventHandler& ucUpgradeEventHandler
    );
    virtual ~ComponentPackageProcessor();

    void Initialize( IPmPlatformDependencies* dep ) override;

    bool PreDownloadedBinaryExists( PmComponent& componentPackage ) override;
    bool HasConfigs( PmComponent& componentPackage ) override;

    bool DownloadPackageBinary( PmComponent& componentPackage ) override;

    bool ProcessPackageBinary( PmComponent& componentPackage ) override;
    bool ProcessConfigsForPackage( PmComponent& componentPackage ) override;

private:
    std::mutex m_mutex;
    IInstallerCacheManager& m_installerManager;
    IFileSysUtil& m_fileUtil;
    ISslUtil& m_sslUtil;
    IPackageConfigProcessor& m_configProcessor;
    IUcidAdapter& m_ucidAdapter;
    ICloudEventBuilder& m_eventBuilder;
    ICloudEventPublisher& m_eventPublisher;
    IUcUpgradeEventHandler& m_ucUpgradeEventHandler;
    IPmPlatformDependencies* m_dependencies;
    uint32_t m_fileCount;

    bool IsPackageFoundLocally( const std::string& nameAndVersion, const std::string& nameOnly, std::string& localVersion );
};

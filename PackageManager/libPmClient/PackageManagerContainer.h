#pragma once

#include <memory>

class IPackageManager;
class IPmHttp;
class IPmCloud;
class IPmConfig;
class IPmManifest;
class IWorkerThread;
class IFileUtil;
class IPackageInventoryProvider;
class IPackageDiscoveryManager;
class ICheckinFormatter;
class IUcidAdapter;
class ICertsAdapter;
class ICheckinManifestRetriever;
class IComponentPackageProcessor;
class IPackageConfigProcessor;
class IManifestProcessor;
class ISslUtil;
class ICloudEventStorage;
class ICloudEventBuilder;
class ICloudEventPublisher;
class IUcUpgradeEventHandler;

class PackageManagerContainer
{
public:
    PackageManagerContainer();
    ~PackageManagerContainer();

    IPackageManager& packageManager();

private:
    std::unique_ptr<IFileUtil> m_fileUtil;
    std::unique_ptr<ISslUtil> m_sslUtil;
    std::unique_ptr<IPmHttp> m_http;
    std::unique_ptr<IPmCloud> m_cloud;
    std::unique_ptr<IPmConfig> m_config;
    std::unique_ptr<IPmManifest> m_manifest;
    std::unique_ptr<IWorkerThread> m_thread;
    std::unique_ptr<IPackageInventoryProvider> m_packageInventoryProvider;
    std::unique_ptr<IPackageDiscoveryManager> m_packageDiscoveryManager;
    std::unique_ptr<ICheckinFormatter> m_checkinFormatter;
    std::unique_ptr<IUcidAdapter> m_ucidAdapter;
    std::unique_ptr<ICertsAdapter> m_certsAdapter;
    std::unique_ptr<ICloudEventStorage> m_eventStorage;
    std::unique_ptr<ICloudEventBuilder> m_eventBuilder;
    std::unique_ptr<ICloudEventPublisher> m_eventPublisher;
    std::unique_ptr<ICloudEventBuilder> m_ucUpgradeEventBuilder;
    std::unique_ptr<ICloudEventStorage> m_ucUpgradeEventStorage;
    std::unique_ptr<IUcUpgradeEventHandler> m_ucUpgradeEventHandler;
    std::unique_ptr<ICheckinManifestRetriever> m_checkinManifestRetriever;
    std::unique_ptr<IPackageConfigProcessor> m_packageConfigProcessor;
    std::unique_ptr<IComponentPackageProcessor> m_componentPackageProcessor;
    std::unique_ptr<IManifestProcessor> m_manifestProcessor;

    std::unique_ptr<IPackageManager> m_pacMan;
};
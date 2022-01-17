#pragma once

#include <memory>

class IPackageManager;
class IPmHttp;
class IPmCloud;
class IPmBootstrap;
class IPmConfig;
class IPmManifest;
class IWorkerThread;
class IFileSysUtil;
class IPackageInventoryProvider;
class IPackageDiscoveryManager;
class ICheckinFormatter;
class IUcidAdapter;
class ICertsAdapter;
class ICatalogJsonParser;
class ICatalogListRetriever;
class ICheckinManifestRetriever;
class IComponentPackageProcessor;
class IPackageConfigProcessor;
class IManifestProcessor;
class ISslUtil;
class ICloudEventStorage;
class ICloudEventBuilder;
class ICloudEventPublisher;
class IUcUpgradeEventHandler;
class IInstallerCacheManager;
class IRebootHandler;
class IUtf8PathVerifier;
class IWatchdog;
class ProxyContainer;
class IProxyDiscovery;
class IProxyConsumer;

class PackageManagerContainer
{
public:
    PackageManagerContainer();
    ~PackageManagerContainer();

    IPackageManager& packageManager();

private:
    std::unique_ptr<IUtf8PathVerifier> m_utfPathVerifier;
    std::unique_ptr<IFileSysUtil> m_fileUtil;
    std::unique_ptr<ISslUtil> m_sslUtil;
    std::unique_ptr<IPmHttp> m_http;
    std::unique_ptr<IPmCloud> m_cloud;
    std::unique_ptr<IInstallerCacheManager> m_installeracheMgr;
    std::unique_ptr<IUcidAdapter> m_ucidAdapter;
    std::unique_ptr<IPmBootstrap> m_bootstrap;
    std::unique_ptr<IPmConfig> m_config;
    std::unique_ptr<ProxyContainer> m_proxyContainer;
    std::unique_ptr<IProxyConsumer> m_proxyDiscoverySubscriber;
    std::unique_ptr<IProxyDiscovery> m_proxyDiscovery;
    std::unique_ptr<IPmManifest> m_manifest;
    std::unique_ptr<IWorkerThread> m_thread;
    std::unique_ptr<IPackageInventoryProvider> m_packageInventoryProvider;
    std::unique_ptr<ICheckinFormatter> m_checkinFormatter;
    std::unique_ptr<ICertsAdapter> m_certsAdapter;
    std::unique_ptr<ICloudEventStorage> m_eventStorage;
    std::unique_ptr<ICloudEventBuilder> m_eventBuilder;
    std::unique_ptr<ICloudEventPublisher> m_eventPublisher;
    std::unique_ptr<ICloudEventBuilder> m_ucUpgradeEventBuilder;
    std::unique_ptr<ICloudEventStorage> m_ucUpgradeEventStorage;
    std::unique_ptr<IUcUpgradeEventHandler> m_ucUpgradeEventHandler;
    std::unique_ptr<ICatalogJsonParser> m_catalogJsonParser;
    std::unique_ptr<ICatalogListRetriever> m_catalogListRetriever;
    std::unique_ptr<ICheckinManifestRetriever> m_checkinManifestRetriever;
    std::unique_ptr<IPackageDiscoveryManager> m_packageDiscoveryManager;
    std::unique_ptr<IPackageConfigProcessor> m_packageConfigProcessor;
    std::unique_ptr<IWatchdog> m_watchdog;
    std::unique_ptr<IComponentPackageProcessor> m_componentPackageProcessor;
    std::unique_ptr<IManifestProcessor> m_manifestProcessor;
    std::unique_ptr<IRebootHandler> m_rebootHandler;

    std::unique_ptr<IPackageManager> m_pacMan;
};
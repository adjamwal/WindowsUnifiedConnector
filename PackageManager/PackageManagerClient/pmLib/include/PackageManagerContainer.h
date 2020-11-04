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
class ICheckinFormatter;
class ITokenAdapter;
class ICertsAdapter;
class ICheckinManifestRetriever;
class IComponentPackageProcessor;
class IPackageConfigProcessor;
class IManifestProcessor;
class ISslUtil;

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
    std::unique_ptr<ICheckinFormatter> m_checkinFormatter;
    std::unique_ptr<ITokenAdapter> m_tokenAdapter;
    std::unique_ptr<ICertsAdapter> m_certsAdapter;
    std::unique_ptr<ICheckinManifestRetriever> m_checkinManifestRetriever;
    std::unique_ptr<IPackageConfigProcessor> m_packageConfigProcessor;
    std::unique_ptr<IComponentPackageProcessor> m_componentPackageProcessor;
    std::unique_ptr<IManifestProcessor> m_manifestProcessor;

    std::unique_ptr<IPackageManager> m_pacMan;
};
#pragma once

#include <memory>

class IPackageManager;
class IPmHttp;
class IPmCloud;
class IPmConfig;
class IPmManifest;
class IWorkerThread;
class IFileUtil;

class PackageManagerContainer
{
public:
    PackageManagerContainer();
    ~PackageManagerContainer();

    IPackageManager& packageManager();

private:
    std::unique_ptr<IFileUtil> m_fileUtil;
    std::unique_ptr <IPmHttp> m_http;
    std::unique_ptr<IPmCloud> m_cloud;
    std::unique_ptr<IPmConfig> m_config;
    std::unique_ptr<IPmManifest> m_manifest;
    std::unique_ptr<IWorkerThread> m_thread;

    std::unique_ptr<IPackageManager> m_pacMan;
};
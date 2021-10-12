#pragma once

#include "IPackageConfigProcessor.h"
#include <mutex>
#include <filesystem>

class IFileSysUtil;
class ISslUtil;
class IUcidAdapter;
class ICloudEventBuilder;
class ICloudEventPublisher;

class PackageConfigProcessor : public IPackageConfigProcessor
{
public:
    PackageConfigProcessor( 
        IFileSysUtil& fileUtil, 
        ISslUtil& sslUtil,
        IUcidAdapter& ucidAdapter,
        ICloudEventBuilder& eventBuilder,
        ICloudEventPublisher& eventPublisher );
    ~PackageConfigProcessor();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool ProcessConfig( PackageConfigInfo& config ) override;

private:
    bool AddConfig( PackageConfigInfo& config );
    bool RemoveConfig( PackageConfigInfo& config );
    void EraseOrThrow( const std::filesystem::path& targetLocation,
                       const std::filesystem::path& unresolvedPath,
                       const std::string& configSha256 );
    void RemoveTempFile( const std::filesystem::path& tempFilePath );

    IFileSysUtil& m_fileUtil;
    ISslUtil& m_sslUtil;
    IUcidAdapter& m_ucidAdapter;
    ICloudEventBuilder& m_eventBuilder;
    ICloudEventPublisher& m_eventPublisher;
    std::mutex m_mutex;
    IPmPlatformDependencies* m_dependencies;
    uint32_t m_fileCount;
};
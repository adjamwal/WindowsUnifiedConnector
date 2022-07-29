#pragma once

#include "IPmManifest.h"
#include <mutex>

namespace Json {
    class Value;
};

class IPmPlatformDependencies;

class PmManifest : public IPmManifest
{
public:
    PmManifest();
    ~PmManifest();

    int32_t ParseManifest( const std::string& manifestJson ) override;
    void SetDeploymentId( const std::string& deploymentId ) override;
    const std::string& GetDeploymentId() override;
    std::vector<PmComponent> GetPackageList() override;
    void Initialize( IPmPlatformDependencies* dep ) override;

private:
    std::mutex m_mutex;
    std::string m_deploymentId;
    std::vector<PmComponent> m_ComponentList;
    IPmPlatformDependencies* m_dependencies;

    void AddPackage( Json::Value& package );
    void AddConfigToPackage( Json::Value& config, PmComponent& package );
    std::string GetJsonStringField( Json::Value& packageJson, const char* field, bool required );
};
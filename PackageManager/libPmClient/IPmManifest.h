#pragma once

#include <string>
#include <vector>

class IPmPlatformDependencies;
struct PmComponent;

class IPmManifest
{
public:
    IPmManifest() {}
    virtual ~IPmManifest() {}

    virtual int32_t ParseManifest( const std::string& manifestJson ) = 0;
    virtual void SetDeploymentId( const std::string& deploymentId ) = 0;
    virtual const std::string& GetDeploymentId() = 0;
    virtual std::vector<PmComponent> GetPackageList() = 0;
    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
};
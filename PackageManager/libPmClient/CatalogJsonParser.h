#pragma once

#include "ICatalogJsonParser.h"
#include "PmTypes.h"
#include <json/json.h>
#include <mutex>

class IPmPlatformDependencies;

class CatalogJsonParser : public ICatalogJsonParser
{
public:
    CatalogJsonParser();
    ~CatalogJsonParser();

    bool Parse( const std::string json, std::vector<PmProductDiscoveryRules>& returnCatalogDataset ) override;
    void Initialize( IPmPlatformDependencies* dep ) override;

private:
    void ParseConfigurables( const Json::Value& pkgValue, std::vector<PmProductDiscoveryConfigurable>& returnPkgConfigs );
    void ParseConfigFormats( const Json::Value& pkgConfigValue, std::vector<std::string>& returnFormats );
    void ParseMsiDiscovery( const Json::Value & msiValue, std::vector<PmProductDiscoveryMsiMethod>&returnMsi );
    void ParseRegistryDiscovery( const Json::Value & regValue, std::vector<PmProductDiscoveryRegistryMethod>&returnRegistry );
    void ParseMsiUpgradeCodeDiscovery( const Json::Value& msiUpgardeValue, std::vector<PmProductDiscoveryMsiUpgradeCodeMethod>& returnMsi );

    std::mutex m_mutex;
    IPmPlatformDependencies* m_dependencies;
};

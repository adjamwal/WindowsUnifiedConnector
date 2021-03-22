#pragma once

#include "ICatalogJsonParser.h"
#include "PmTypes.h"
#include <json/json.h>

struct PmDiscoveryConfigurable;

class CatalogJsonParser : public ICatalogJsonParser
{
public:
    CatalogJsonParser() {}
    virtual ~CatalogJsonParser() {}

    bool Parse( const std::string json, std::vector<PmProductDiscoveryRules>& returnCatalogDataset ) override;
private:
    void ParseConfigurables( const Json::Value& pkgValue, std::vector<PmProductDiscoveryConfigurable>& returnPkgConfigs );
    void ParseConfigFormats( const Json::Value& pkgConfigValue, std::vector<std::string>& returnFormats );
    void ParseMsiDiscovery( const Json::Value & msiValue, std::vector<PmProductDiscoveryMsiMethod>&returnMsi );
    void ParseRegistryDiscovery( const Json::Value & regValue, std::vector<PmProductDiscoveryRegistryMethod>&returnRegistry );
    void ParseMsiUpgradeCodeDiscovery( const Json::Value& msiUpgardeValue, std::vector<PmProductDiscoveryMsiUpgradeCodeMethod>& returnMsi );
};

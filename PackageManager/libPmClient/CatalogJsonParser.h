#pragma once

#include "ICatalogJsonParser.h"
#include <json/json.h>

struct PmDiscoveryConfigurable;

class CatalogJsonParser : public ICatalogJsonParser
{
public:
    CatalogJsonParser() {}
    virtual ~CatalogJsonParser() {}

    bool Parse( const std::string json, std::vector<PmDiscoveryComponent>& returnCatalogDataset ) override;
private:
    void ParseConfigurables( const Json::Value& pkgValue, std::vector<PmDiscoveryConfigurable>& returnPkgConfigs );
    void ParseConfigFormats( const Json::Value& pkgConfigValue, std::vector<std::string>& returnFormats );
};

#pragma once

#include <string>
#include <vector>

struct PmProductDiscoveryRules;

class ICatalogJsonParser
{
public:
    ICatalogJsonParser() {}
    virtual ~ICatalogJsonParser() {}

    virtual bool Parse( const std::string json, std::vector<PmProductDiscoveryRules>& returnCatalogDataset ) = 0;
};

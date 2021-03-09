#pragma once

#include <string>
#include <vector>

struct PmDiscoveryComponent;

class ICatalogJsonParser
{
public:
    ICatalogJsonParser() {}
    virtual ~ICatalogJsonParser() {}

    virtual bool Parse(const std::string json, std::vector<PmDiscoveryComponent>& catalogDataset ) = 0;
};

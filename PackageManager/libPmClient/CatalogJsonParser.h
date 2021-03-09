#pragma once

#include "ICatalogJsonParser.h"

class CatalogJsonParser : public ICatalogJsonParser
{
public:
    CatalogJsonParser() {}
    virtual ~CatalogJsonParser() {}

    bool Parse( const std::string json, std::vector<PmDiscoveryComponent>& catalogDataset ) override;
};

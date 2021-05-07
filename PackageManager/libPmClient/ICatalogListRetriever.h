#pragma once

#include <string>

class ICatalogListRetriever
{
public:
    ICatalogListRetriever() {}
    virtual ~ICatalogListRetriever() {}

    virtual std::string GetCloudCatalog() = 0;
};

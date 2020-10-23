#pragma once

#include "IPmManifest.h"
#include <mutex>

namespace Json {
    class Value;
};

class PmManifest : public IPmManifest
{
public:
    PmManifest();
    ~PmManifest();

    int32_t ParseManifest( const std::string& manifestJson ) override;
    std::vector<PmComponent> GetPackageList() override;

private:
    std::mutex m_mutex;
    std::vector<PmComponent> m_ComponentList;

    void AddPackage( Json::Value& package );
    std::string GetJsonStringField( Json::Value& packageJson, const char* field, bool required );
};
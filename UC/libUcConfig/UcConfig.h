#pragma once

#include "IUcConfig.h"
#include <mutex>
#include <json/json.h>

class UcConfig : public IUcConfig
{
public:
    UcConfig();
    ~UcConfig();

    bool LoadConfig() override;
    bool VerifyConfig( const std::wstring& path = L"" ) override;

    uint32_t GetLogLevel() override;

private:
    std::mutex m_mutex;
    uint32_t m_logLevel;
    uint32_t m_fileModifyTime;
    std::wstring m_path;

    Json::Value GetJsonFromFile( const std::wstring& path );
};
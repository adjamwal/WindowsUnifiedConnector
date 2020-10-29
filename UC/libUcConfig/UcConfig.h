#pragma once

#include "IUcConfig.h"
#include <mutex>

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
    std::wstring m_path;
};
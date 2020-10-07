#pragma once

#include <string>
#include <mutex>

#include "IPmCloud.h"

class PmCloud : public IPmCloud
{
public:
    PmCloud();
    ~PmCloud();

    void SetUri( const std::string& uri ) override;
    void SetToken( const std::string& token ) override;
    int32_t Checkin( const std::string& payload, std::string& response ) override;
    int32_t SendEvent( const std::string& payload ) override;
    int32_t DownloadFile( const std::string& uri, const std::string filename ) override;

private:
    std::string m_uri;
    std::string m_token;
    std::mutex m_mutex;
};
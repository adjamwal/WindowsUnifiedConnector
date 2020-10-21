#pragma once

#include <string>
#include <mutex>

#include "IPmCloud.h"
#include "IPmHttp.h"

class PmCloud : public IPmCloud
{
public:
    PmCloud( IPmHttp& http );
    ~PmCloud();

    void SetUri( const std::string& uri ) override;
    void SetToken( const std::string& token ) override;
    void SetCerts( const PmHttpCertList& certs ) override;
    int32_t Checkin( const std::string& payload, std::string& response ) override;
    int32_t SendEvent( const std::string& payload ) override;
    int32_t DownloadFile( const std::string& uri, const std::string filename ) override;

private:
    IPmHttp& m_http;
    PmHttpCertList m_certs;
    std::string m_uri;
    std::string m_token;
    std::mutex m_mutex;
};
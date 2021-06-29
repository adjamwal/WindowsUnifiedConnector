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
    void SetUserAgent( const std::string& userAgent ) override;
    void SetShutdownFunc( std::function<bool()> shutdownFunc ) override;
    int32_t Checkin( const std::string& payload, std::string& response ) override;
    int32_t Get( const std::string& url, std::string& response, int32_t& httpReturn ) override;
    int32_t Post( const std::string& url, void* payload, size_t payloadSize, std::string& response, int32_t& httpReturn ) override;
    int32_t DownloadFile( const std::string& uri, const std::filesystem::path& filename ) override;

private:
    IPmHttp& m_http;
    PmHttpCertList m_certs;
    std::string m_uri;
    std::string m_token;
    std::string m_userAgent;
    std::function<bool()> m_shutdownFunc;
    std::mutex m_mutex;

    int ProgressCallback( PM_TYPEOF_OFF_T dltotal, PM_TYPEOF_OFF_T dlnow, PM_TYPEOF_OFF_T ultotal, PM_TYPEOF_OFF_T ulnow );
    static int _ProgressCallback( void* clientp, PM_TYPEOF_OFF_T dltotal, PM_TYPEOF_OFF_T dlnow, PM_TYPEOF_OFF_T ultotal, PM_TYPEOF_OFF_T ulnow );
};

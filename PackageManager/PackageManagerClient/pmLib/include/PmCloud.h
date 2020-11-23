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
    void SetUserAgent( const std::string& agent ) override;
    void SetShutdownFunc( std::function<bool()> shutdownFunc ) override;
    int32_t Checkin( const std::string& payload, std::string& response ) override;
    int32_t SendEvent( const std::string& payload ) override;
    int32_t DownloadFile( const std::string& uri, const std::string filename ) override;

private:
    IPmHttp& m_http;
    PmHttpCertList m_certs;
    std::string m_uri;
    std::string m_token;
    std::string m_agent;
    std::function<bool()> m_shutdownFunc;
    std::mutex m_mutex;

    int ProgressCallback( PM_TYPEOF_OFF_T dltotal, PM_TYPEOF_OFF_T dlnow, PM_TYPEOF_OFF_T ultotal, PM_TYPEOF_OFF_T ulnow );
    static int _ProgressCallback( void* clientp, PM_TYPEOF_OFF_T dltotal, PM_TYPEOF_OFF_T dlnow, PM_TYPEOF_OFF_T ultotal, PM_TYPEOF_OFF_T ulnow );
};
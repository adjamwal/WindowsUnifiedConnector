#pragma once

#include <string>
#include <mutex>

#include "IPmCloud.h"
#include "IPmHttp.h"

class IPmPlatformConfiguration;

class PmCloud : public IPmCloud
{
public:
    PmCloud( IPmHttp& http );
    ~PmCloud();

    void Initialize( IPmPlatformDependencies* dependencies );
    void SetCheckinUri( const std::string& uri ) override;
    void SetToken( const std::string& token ) override;
    void SetCerts( const PmHttpCertList& certs ) override;
    void SetUserAgent( const std::string& userAgent ) override;
    void SetShutdownFunc( std::function<bool()> shutdownFunc ) override;
    bool Checkin( const std::string& payload, std::string& responseContent, PmHttpExtendedResult& eResult ) override;
    bool Get( const std::string& url, std::string& responseContent, PmHttpExtendedResult& eResult ) override;
    bool Post( const std::string& url, const void* payload, size_t payloadSize, std::string& responseContent, PmHttpExtendedResult& eResult ) override;
    bool DownloadFile( const std::string& uri, const std::filesystem::path& filename, PmHttpExtendedResult& eResult ) override;

private:
    IPmHttp& m_http;
    PmHttpCertList m_certs;
    IPmPlatformConfiguration* m_deps;

    std::string m_checkinUri;
    std::string m_token;
    std::string m_userAgent;
    std::function<bool()> m_shutdownFunc;
    std::mutex m_mutex;

    int ProgressCallback( PM_TYPEOF_OFF_T dltotal, PM_TYPEOF_OFF_T dlnow, PM_TYPEOF_OFF_T ultotal, PM_TYPEOF_OFF_T ulnow );
    static int _ProgressCallback( void* clientp, PM_TYPEOF_OFF_T dltotal, PM_TYPEOF_OFF_T dlnow, PM_TYPEOF_OFF_T ultotal, PM_TYPEOF_OFF_T ulnow );
};

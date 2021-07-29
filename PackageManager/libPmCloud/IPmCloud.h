#pragma once

#include <string>
#include <functional>
#include <filesystem>

struct PmHttpCertList;
struct PmHttpExtendedResult;

class IPmCloud
{
public:
    IPmCloud() {}
    virtual ~IPmCloud() {}

    virtual void SetCheckinUri( const std::string& uri ) = 0;
    virtual void SetToken( const std::string& token ) = 0;
    virtual void SetCerts( const PmHttpCertList& certs ) = 0;
    virtual void SetUserAgent( const std::string& userAgent ) = 0;
    virtual void SetShutdownFunc( std::function<bool()> shutdownFunc ) = 0;
    virtual bool Checkin( const std::string& payload, std::string& responseContent, PmHttpExtendedResult& eResult ) = 0;
    virtual bool Get( const std::string& url, std::string& responseContent, PmHttpExtendedResult& eResult ) = 0;
    virtual bool Post( const std::string& url, const void* payload, size_t payloadSize, std::string& responseContent, PmHttpExtendedResult& eResult ) = 0;
    virtual bool DownloadFile( const std::string& uri, const std::filesystem::path& filename, PmHttpExtendedResult& eResult ) = 0;
};

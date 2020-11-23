#pragma once

#include <cstdarg>
#include <string>
#include <openssl/x509.h>

struct PmHttpCertList
{
    X509** certificates;
    size_t count;
};

class IHttpAdapter
{
public:
    IHttpAdapter() {}
    virtual ~IHttpAdapter() {}

    virtual int32_t Init( void* ctx, const std::string& userAgent ) = 0;
    virtual int32_t Deinit() = 0;
    virtual int32_t SetCerts( const PmHttpCertList& cert ) = 0;
    virtual int32_t HttpGet( const std::string &url, std::string &response, int32_t &httpReturn ) = 0;
    virtual int32_t HttpPost( const std::string& url, void* data, size_t dataSize, std::string& response, int32_t &httpReturn ) = 0;
};

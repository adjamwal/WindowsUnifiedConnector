#pragma once

#include <cstdarg>
#include <string>
#include <openssl/x509.h>

#if _WIN32 || _WIN64
#if _WIN64
#   define PM_TYPEOF_OFF_T  __int64
#else
#   define PM_TYPEOF_OFF_T  long
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#   define PM_TYPEOF_OFF_T  __int64
#else
#   define PM_TYPEOF_OFF_T  long
#endif
#endif

struct PmHttpCertList
{
    X509** certificates;
    size_t count;
};

class IPmHttp
{
public:
    typedef int ( *PM_PROGRESS_CALLBACK )( void* clientp, PM_TYPEOF_OFF_T dltotal, PM_TYPEOF_OFF_T dlnow, PM_TYPEOF_OFF_T ultotal, PM_TYPEOF_OFF_T ulnow );

    IPmHttp() {}
    virtual ~IPmHttp() {}

    virtual int32_t Init( PM_PROGRESS_CALLBACK callback, void* ctx, const std::string& userAgent ) = 0;
    virtual int32_t Deinit() = 0;
    virtual int32_t SetToken( const std::string& token ) = 0;
    virtual int32_t SetCerts( const PmHttpCertList& cert ) = 0;
    virtual int32_t HttpGet( const std::string &url, std::string &response, int32_t &httpReturn ) = 0;
    virtual int32_t HttpPost( const std::string& url, void* data, size_t dataSize, std::string& response, int32_t &httpReturn ) = 0;
    virtual int32_t HttpDownload( const std::string& url, const std::string& filepath, int32_t &httpReturn ) = 0;
};
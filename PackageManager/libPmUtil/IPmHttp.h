#pragma once

#include <cstdarg>
#include <string>
#include <openssl/x509.h>
#include <filesystem>
#include "PmTypes.h"

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

enum class USER_DEF_SUB_ERROR {
    SC_MISSING_TOKEN    = 0x50000001
};

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

    virtual bool Init( PM_PROGRESS_CALLBACK callback, void* ctx, const std::string& userAgent, PmHttpExtendedResult& eResult ) = 0;
    virtual bool Deinit() = 0;
    virtual bool SetToken( const std::string& token, PmHttpExtendedResult& eResult ) = 0;
    virtual bool SetCerts( const PmHttpCertList& cert, PmHttpExtendedResult& eResult ) = 0;
    virtual bool HttpGet( const std::string &url, std::string &responseContent, PmHttpExtendedResult& eResult ) = 0;
    virtual bool HttpPost( const std::string& url, const void* data, size_t dataSize, std::string& responseContent, PmHttpExtendedResult& eResult ) = 0;
    virtual bool HttpDownload( const std::string& url, const std::filesystem::path& filepath, PmHttpExtendedResult& eResult ) = 0;
};

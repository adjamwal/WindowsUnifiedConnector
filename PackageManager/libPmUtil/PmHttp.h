#pragma once

#include "IPmHttp.h"
#include <curl/curl.h>
#include <mutex>

class IFileSysUtil;

struct FileUtilHandle;

class PmHttp : public IPmHttp
{
public:
    PmHttp( IFileSysUtil& fileUtil );
    ~PmHttp();

    int32_t Init( PM_PROGRESS_CALLBACK callback, void* ctx, const std::string& agent ) override;
    int32_t Deinit() override;
    int32_t SetToken( const std::string& token ) override;
    int32_t SetCerts( const PmHttpCertList& cert ) override;
    int32_t HttpGet( const std::string& url, std::string& response, int32_t &httpReturn ) override;
    int32_t HttpPost( const std::string& url, void* data, size_t dataSize, std::string& response, int32_t &httpReturn ) override;
    int32_t HttpDownload( const std::string& url, const std::string& filepath, int32_t &httpReturn ) override;

private:
    IFileSysUtil& m_fileUtil;
    CURL *m_curlHandle;
    std::string m_userAgent;
    std::mutex m_mutex;
    std::string m_token;
    struct curl_slist* m_headerList;
    PmHttpCertList m_certList;

    struct WriteFileCtx {
        IFileSysUtil* fileUtil;
        FileUtilHandle* handle;
    };

    void FreeCerts();

    static size_t WriteString( void* ptr, size_t size, size_t nmemb, std::string* data );
    static size_t WriteFile( void* ptr, size_t size, size_t nmemb, WriteFileCtx* data );
    static CURLcode SslCallback( CURL* curl, void* sslctx, void* param );
    static int X509_subj_name( const X509* cert, char** subj_name );
};
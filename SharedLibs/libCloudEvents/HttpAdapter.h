#pragma once

#include "IHttpAdapter.h"
#include <curl/curl.h>
#include <mutex>

class HttpAdapter final : public IHttpAdapter
{
public:
    HttpAdapter();
    ~HttpAdapter();

    int32_t Init( void* ctx, const std::string& agent ) override;
    int32_t Deinit() override;
    int32_t SetCerts( const PmHttpCertList& cert ) override;
    int32_t HttpGet( const std::string& url, std::string& response, int32_t &httpReturn ) override;
    int32_t HttpPost( const std::string& url, void* data, size_t dataSize, std::string& response, int32_t &httpReturn ) override;

private:
    CURL *m_curlHandle;
    std::string m_userAgent;
    std::mutex m_mutex;
    struct curl_slist* m_headerList;
    PmHttpCertList m_certList;

    void FreeCerts();

    static size_t WriteString( void* ptr, size_t size, size_t nmemb, std::string* data );
    static CURLcode SslCallback( CURL* curl, void* sslctx, void* param );
    static int X509_subj_name( const X509* cert, char** subj_name );
};

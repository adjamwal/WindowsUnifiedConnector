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

    bool Init( PM_PROGRESS_CALLBACK callback, void* ctx, const std::string& agent, PmHttpExtendedResult& eResult ) override;
    bool Deinit() override;
    bool SetToken( const std::string& token, PmHttpExtendedResult& eResult ) override;
    bool SetCerts( const PmHttpCertList& cert, PmHttpExtendedResult& eResult ) override;
    void SetHttpProxy( const std::string& proxyUri, const std::string& proxyUserName, const std::string& proxyPassword ) override;
    bool HttpGet( const std::string& url, std::string& responseContent, PmHttpExtendedResult& eResult ) override;
    bool HttpPost( const std::string& url, const void* data, size_t dataSize, std::string& responseContent, PmHttpExtendedResult& eResult ) override;
    bool HttpDownload( const std::string& url, const std::filesystem::path& filepath, PmHttpExtendedResult& eResult ) override;
    bool IsSslPeerValidationError( PmHttpExtendedResult& eResult ) override;
    std::string GetErrorDescription( int errorCode ) override;

private:
    IFileSysUtil& m_fileUtil;
    CURL *m_curlHandle;
    std::string m_userAgent;
    std::mutex m_mutex;
    std::string m_token;
    struct curl_slist* m_headerList;
    PmHttpCertList m_certList;
    std::string m_proxyuri;
    std::string m_proxyuser;
    std::string m_proxypass;
    char m_errbuf[ CURL_ERROR_SIZE ];

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

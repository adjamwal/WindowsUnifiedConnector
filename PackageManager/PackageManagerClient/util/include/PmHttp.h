#include "IPmHttp.h"
#include <curl/curl.h>
#include <mutex>

class IFileUtil;

struct FileUtilHandle;

class PmHttp : public IPmHttp
{
public:
    PmHttp( IFileUtil& fileUtil );
    ~PmHttp();

    int32_t Init( PM_PROGRESS_CALLBACK callback, void* ctx, const std::string& agent ) override;
    int32_t Deinit() override;
    int32_t HttpGet( const std::string& url, std::string& response, int32_t &httpReturn ) override;
    int32_t HttpPost( const std::string& url, void* data, size_t dataSize, std::string& response, int32_t &httpReturn ) override;
    int32_t HttpDownload( const std::string& url, std::string& filepath, int32_t &httpReturn ) override;

private:
    IFileUtil& m_fileUtil;
    CURL *m_curlHandle;
    std::string m_agent;
    std::mutex m_mutex;

    struct WriteFileCtx {
        IFileUtil* fileUtil;
        FileUtilHandle* handle;
    };

    static size_t WriteString( void* ptr, size_t size, size_t nmemb, std::string* data );
    static size_t WriteFile( void* ptr, size_t size, size_t nmemb, WriteFileCtx* data );
};
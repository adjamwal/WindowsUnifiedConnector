// FakeUCB.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "httpmockserver/mock_server.h"
#include "httpmockserver/test_environment.h"
#include "FileSysUtil.h"
#include "Utf8PathVerifier.h"
#include <cstdio>

class FakeUcbServer: public httpmock::MockServer
{
public:
    explicit FakeUcbServer(
        const std::string& workingDir = "",
        int port = 9201,
        int connectionHangTimeMs = 30000);
    virtual ~FakeUcbServer();

private:
    virtual Response responseHandler(
        const std::string& url,
        const std::string& method,
        const std::string& data,
        const std::vector<UrlArg>& urlArguments,
        const std::vector<Header>& headers) override;

    Response GetJsonResponse(const std::string& filename);
    Response GetFileResponse(const std::string& filename);

    int m_connectionHangTime;
    std::string m_workingDir;
};

FakeUcbServer::FakeUcbServer(
    const std::string& workingDir,
    int port,
    int connectionHangTimeMs)
    : MockServer(port)
    , m_connectionHangTime(connectionHangTimeMs)
    , m_workingDir( workingDir )
{
}


FakeUcbServer::~FakeUcbServer()
{
}

httpmock::MockServer::Response FakeUcbServer::GetJsonResponse(const std::string& filename)
{
    Utf8PathVerifier pathVerifier;
    FileSysUtil fsUtil(pathVerifier);
    std::filesystem::path filePath;

    if (m_workingDir.empty()) {
        filePath = filename;
    }
    else {
        filePath = m_workingDir;
        filePath /= filename;
    }

    printf("Reading File %s\n", filePath.string().c_str());

    Response resp(200, fsUtil.ReadFile(filePath));
    resp.addHeader(Header("Content-Type", "application/json"));
    return resp;
}

httpmock::MockServer::Response FakeUcbServer::GetFileResponse(const std::string& filename)
{
    std::filesystem::path filePath;
    std::string rtn;

    if (m_workingDir.empty()) {
        filePath = filename;
    }
    else {
        filePath = m_workingDir;
        filePath /= filename;
    }

    printf("Reading File %s\n", filePath.string().c_str());

    FILE* fp = NULL;
    if (fopen_s(&fp, filePath.string().c_str(), "rb") == 0 && fp) {
        fseek(fp, 0L, SEEK_END);
        size_t sz = ftell(fp);
        fseek(fp, 0L, SEEK_SET);

        rtn.resize(sz);
        fread_s(( void* )rtn.data(), sz, 1, sz, fp);
        fclose(fp);
    }

    // meh... hacking file contents into a string to make it fit the MockServer API
    Response resp(200, rtn);
    resp.addHeader(Header("Content-Type", "application/octet-stream"));
    return resp;
}

httpmock::MockServer::Response FakeUcbServer::responseHandler(
    const std::string& url,
    const std::string& method,
    const std::string& data,
    const std::vector<UrlArg>& urlArguments /*=*/,
    const std::vector<Header>& headers /*= */)
{
    printf("\nRecieved %s: %s\n", method.c_str(), url.c_str());
    if (method == "POST" && url == "/checkin") {
        printf("Handling checkin request\n");
        return GetJsonResponse("checkin.json");
    }
    else if (method == "GET" && url == "/catalog") {
        printf("Handling catalog request\n");
        return GetJsonResponse("catalog.json");
    }
    else if (method == "POST" && url == "/event/1") {
        printf("Handling event request\n");
        return GetJsonResponse("event.json");
    }
    else if (method == "GET" && url.substr(0, 6) == "/file/") {
        printf("Handling file request\n");
        return GetFileResponse(url.substr(6));
    }

    return Response(200, "OK");
}

void PrintHelp()
{
    printf(
R"(FakeUCB.exe

Fake UCB webserver used for development testing. This fakes out the
 checkin/catalog/event URLs and returns canned responses from files
 in the working directory (checkin.json/catalog.json/event.json)

Usage

    FakeUCB.exe [-d WorkingDirectory]

    -d Sets the working directory

    -h prints this menu

)");
}

int main(int argc, char **argv)
{
    std::string workingDir;

    if (argc > 1) {
        for (int i = 0; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-h") {
                PrintHelp();
                return 0;
            }

            if (arg == "-d") {
                if (i != (argc - 1)) {
                    workingDir = argv[i + 1];
                    i++;
                }
                else {
                    printf("Invalid usage");
                    PrintHelp();
                    return -1;
                }
            }
        }
    }

    FakeUcbServer fakeUcb(workingDir);
    fakeUcb.start();

    printf("Press any key to exit");
    getchar();
}

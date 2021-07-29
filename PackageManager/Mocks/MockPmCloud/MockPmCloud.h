#pragma once
#include "MocksCommon.h"

#include "IPmCloud.h"
#include "IPmHttp.h"

class MockPmCloud : public IPmCloud
{
public:
    MockPmCloud();
    ~MockPmCloud();

    MOCK_METHOD1( SetCheckinUri, void( const std::string& ) );
    void ExpectSetCheckinUriIsNotCalled();

    MOCK_METHOD1( SetToken, void( const std::string& ) );
    void ExpectSetTokenIsNotCalled();

    MOCK_METHOD1( SetCerts, void( const PmHttpCertList& ) );
    void ExpectSetCertsIsNotCalled();

    MOCK_METHOD1( SetUserAgent, void( const std::string& ) );
    void ExpectSetUserAgentIsNotCalled();

    MOCK_METHOD1( SetShutdownFunc, void( std::function<bool()> shutdownFunc ) );
    void ExpectSetShutdownFuncIsNotCalled();

    MOCK_METHOD3( Checkin, bool( const std::string&, std::string&, PmHttpExtendedResult& ) );
    void MakeCheckinReturn( bool retval, const std::string& responseContent, const PmHttpExtendedResult& eResult );
    void ExpectCheckinIsNotCalled();

    MOCK_METHOD3( Get, bool( const std::string&, std::string&, PmHttpExtendedResult& ) );
    void MakeGetReturn( bool retval, const std::string& responseContent, const PmHttpExtendedResult& eResult );
    void ExpectGetIsNotCalled();

    MOCK_METHOD5( Post, bool( const std::string&, const void*, size_t, std::string&, PmHttpExtendedResult& ) );
    void MakePostReturn( bool retval, const std::string& responseContent, const PmHttpExtendedResult& eResult );
    void ExpectPostIsNotCalled();

    MOCK_METHOD3( DownloadFile, bool( const std::string&, const std::filesystem::path&, PmHttpExtendedResult& ) );
    void MakeDownloadFileReturn( bool retval, const PmHttpExtendedResult& eResult );
    void ExpectDownloadFileIsNotCalled();

};

#pragma once
#include "MocksCommon.h"

#include "IPmCloud.h"
#include "IPmHttp.h"

class MockPmCloud : public IPmCloud
{
public:
    MockPmCloud();
    ~MockPmCloud();

    MOCK_METHOD1( SetUri, void( const std::string& ) );
    void ExpectSetUriIsNotCalled();

    MOCK_METHOD1( SetToken, void( const std::string& ) );
    void ExpectSetTokenIsNotCalled();

    MOCK_METHOD1( SetCerts, void( const PmHttpCertList& ) );
    void ExpectSetCertsIsNotCalled();

    MOCK_METHOD1( SetUserAgent, void( const std::string& ) );
    void ExpectSetUserAgentIsNotCalled();

    MOCK_METHOD1( SetShutdownFunc, void( std::function<bool()> shutdownFunc ) );
    void ExpectSetShutdownFuncIsNotCalled();

    MOCK_METHOD2( Checkin, int32_t( const std::string&, std::string& ) );
    void MakeCheckinReturn( int32_t value );
    void ExpectCheckinIsNotCalled();

    MOCK_METHOD1( SendEvent, int32_t( const std::string& ) );
    void MakeSendEventReturn( int32_t value );
    void ExpectSendEventIsNotCalled();

    MOCK_METHOD2( DownloadFile, int32_t( const std::string&, const std::string ) );
    void MakeDownloadFileReturn( int32_t value );
    void ExpectDownloadFileIsNotCalled();

};

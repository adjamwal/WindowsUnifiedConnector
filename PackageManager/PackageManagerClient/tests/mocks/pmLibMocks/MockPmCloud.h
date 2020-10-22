#pragma once
#include "MocksCommon.h"

#include "IPmCloud.h"

class MockPmCloud : public IPmCloud
{
public:
    MockPmCloud();
    ~MockPmCloud();

    MOCK_METHOD1( SetUri, void( std::string& ) );
    void ExpectSetUriIsNotCalled();

    MOCK_METHOD1( SetToken, void( std::string& ) );
    void ExpectSetTokenIsNotCalled();

    MOCK_METHOD1( SetCerts, void( const PmHttpCertList& ) );
    void ExpectSetCertsIsNotCalled();

    MOCK_METHOD2( Checkin, int32_t( std::string&, std::string& ) );
    void MakeCheckinReturn( int32_t value );
    void ExpectCheckinIsNotCalled();

    MOCK_METHOD1( SendEvent, int32_t( std::string& ) );
    void MakeSendEventReturn( int32_t value );
    void ExpectSendEventIsNotCalled();

    MOCK_METHOD2( DownloadFile, int32_t( std::string&, std::string ) );
    void MakeDownloadFileReturn( int32_t value );
    void ExpectDownloadFileIsNotCalled();

};

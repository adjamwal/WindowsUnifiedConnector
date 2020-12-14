#pragma once
#include "MocksCommon.h"

#include "IPmHttp.h"

class MockPmHttp : public IPmHttp
{
public:
    MockPmHttp();
    ~MockPmHttp();

    MOCK_METHOD3( Init, int32_t( PM_PROGRESS_CALLBACK, void*, const std::string& ) );
    void MakeInitReturn( int32_t value );
    void ExpectInitIsNotCalled();

    MOCK_METHOD0( Deinit, int32_t() );
    void MakeDeinitReturn( int32_t value );
    void ExpectDeinitIsNotCalled();

    MOCK_METHOD1( SetToken, int32_t( const std::string& ) );
    void MakeSetTokenReturn( int32_t value );
    void ExpectSetTokenIsNotCalled();

    MOCK_METHOD1( SetCerts, int32_t( const PmHttpCertList& ) );
    void MakeSetCertsReturn( int32_t value );
    void ExpectSetCertsIsNotCalled();

    MOCK_METHOD3( HttpGet, int32_t( const std::string&, std::string&, int32_t& ) );
    void MakeHttpGetReturn( int32_t value );
    void ExpectHttpGetIsNotCalled();

    MOCK_METHOD5( HttpPost, int32_t( const std::string&, void*, size_t, std::string&, int32_t& ) );
    void MakeHttpPostReturn( int32_t value );
    void MakeHttpPostReturn( int32_t value, int32_t httpResponse );
    void ExpectHttpPostIsNotCalled();

    MOCK_METHOD3( HttpDownload, int32_t( const std::string&, const std::string&, int32_t& ) );
    void MakeHttpDownloadReturn( int32_t value );
    void ExpectHttpDownloadIsNotCalled();

};

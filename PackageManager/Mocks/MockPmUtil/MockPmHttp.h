#pragma once
#include "MocksCommon.h"

#include "IPmHttp.h"

class MockPmHttp : public IPmHttp
{
public:
    MockPmHttp();
    ~MockPmHttp();

    MOCK_METHOD4( Init, bool( PM_PROGRESS_CALLBACK, void*, const std::string&, PmHttpExtendedResult& ) );
    void MakeInitReturn( bool retval, const PmHttpExtendedResult& eResult );
    void ExpectInitIsNotCalled();

    MOCK_METHOD0( Deinit, bool() );
    void MakeDeinitReturn( bool retval );
    void ExpectDeinitIsNotCalled();

    MOCK_METHOD2( SetToken, bool( const std::string&, PmHttpExtendedResult& ) );
    void MakeSetTokenReturn( bool retval, const PmHttpExtendedResult& eResult );
    void ExpectSetTokenIsNotCalled();

    MOCK_METHOD2( SetCerts, bool( const PmHttpCertList&, PmHttpExtendedResult& ) );
    void MakeSetCertsReturn( bool retval, const PmHttpExtendedResult& eResult );
    void ExpectSetCertsIsNotCalled();

    MOCK_METHOD3( SetHttpProxy, void( const std::string&, const std::string&, const std::string& ) );
    void ExpectSetHttpProxyIsNotCalled();

    MOCK_METHOD3( HttpGet, bool( const std::string&, std::string&, PmHttpExtendedResult& ) );
    void MakeHttpGetReturn( bool retval, const std::string& responseContent, const PmHttpExtendedResult& eResult );
    void ExpectHttpGetIsNotCalled();

    MOCK_METHOD5( HttpPost, bool( const std::string&, const void*, size_t, std::string&, PmHttpExtendedResult& ) );
    void MakeHttpPostReturn( bool retval, const std::string& responseContent, const PmHttpExtendedResult& eResult );
    void ExpectHttpPostIsNotCalled();

    MOCK_METHOD3( HttpDownload, bool( const std::string&, const std::filesystem::path&, PmHttpExtendedResult& ) );
    void MakeHttpDownloadReturn( bool retval, const PmHttpExtendedResult& eResult );
    void ExpectHttpDownloadIsNotCalled();

    MOCK_METHOD1( IsSslPeerValidationError, bool( PmHttpExtendedResult& ) );
    void MakeIsSslPeerValidationErrorReturn( bool retval );
    void ExpectIsSslPeerValidationErrorIsNotCalled();

    MOCK_METHOD1( GetErrorDescription, std::string( int ) );
};


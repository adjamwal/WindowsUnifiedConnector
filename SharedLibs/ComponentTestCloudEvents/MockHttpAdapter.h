#pragma once

#include "pch.h"
#include "IHttpAdapter.h"

class MockHttpAdapter : public IHttpAdapter
{
public:
    MockHttpAdapter();
    ~MockHttpAdapter();

    MOCK_METHOD2( Init, int32_t( void*, const std::string& ) );
    void MakeInitReturn( int32_t value );
    void ExpectInitIsNotCalled();

    MOCK_METHOD0( Deinit, int32_t() );
    void MakeDeinitReturn( int32_t value );
    void ExpectDeinitIsNotCalled();

    MOCK_METHOD1( SetCerts, int32_t( const PmHttpCertList& ) );
    void MakeSetCertsReturn( int32_t value );
    void ExpectSetCertsIsNotCalled();

    MOCK_METHOD3( HttpGet, int32_t( const std::string&, std::string&, int32_t& ) );
    void MakeHttpGetReturn( int32_t value );
    void ExpectHttpGetIsNotCalled();

    MOCK_METHOD5( HttpPost, int32_t( const std::string&, void*, size_t, std::string&, int32_t& ) );
    void MakeHttpPostReturn( int32_t value );
    void ExpectHttpPostIsNotCalled();

};

#pragma once

#include "MocksCommon.h"
#include "IPmBootstrap.h"

class MockPmBootstrap : public IPmBootstrap
{
public:
    MockPmBootstrap();
    ~MockPmBootstrap();

    MOCK_METHOD1( LoadPmBootstrap, int32_t( const std::string& ) );
    void MakeLoadPmBootstrapReturn( int32_t value );
    void ExpectLoadPmBootstrapIsNotCalled();

    MOCK_METHOD0( GetIdentifyUri, std::string() );
    void MakeGetIdentifyUriReturn( std::string value );
    void ExpectGetIdentifyUriIsNotCalled();
};
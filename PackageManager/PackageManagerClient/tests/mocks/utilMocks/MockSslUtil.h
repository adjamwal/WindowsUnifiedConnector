#pragma once
#include "MocksCommon.h"
#include "ISslUtil.h"

class MockSslUtil : public ISslUtil
{
public:
    MockSslUtil();
    ~MockSslUtil();

    MOCK_METHOD2( DecodeBase64, int32_t( const std::string&, std::vector<uint8_t>& ) );
    void MakeDecodeBase64Return( int32_t value );
    void ExpectDecodeBase64NotCalled();
    
    MOCK_METHOD1( CalculateSHA256, std::optional<std::string>( const std::string filename ) );
    void MakeCalculateSHA256Return( std::optional<std::string> value );
    void ExpectCalculateSHA256NotCalled();
};
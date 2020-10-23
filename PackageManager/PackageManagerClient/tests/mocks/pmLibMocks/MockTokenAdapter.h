#pragma once
#include "MocksCommon.h"

#include "ITokenAdapter.h"

class MockTokenAdapter : public ITokenAdapter
{
public:
    MockTokenAdapter();
    ~MockTokenAdapter();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* ) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD0( GetUcidToken, std::string() );
    void MakeGetUcidTokenReturn( std::string value );
    void ExpectGetUcidTokenIsNotCalled();

};

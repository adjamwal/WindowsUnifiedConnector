#pragma once
#include "MocksCommon.h"

#include "IPmConfig.h"
#include "PmTypes.h"

class MockPmConfig : public IPmConfig
{
public:
    MockPmConfig();
    ~MockPmConfig();

    MOCK_METHOD1( Load, int32_t( const std::string& ) );
    void MakeLoadReturn( int32_t value );
    void ExpectLoadIsNotCalled();

    MOCK_METHOD1( VerifyFileIntegrity, int32_t( const std::string& ) );
    void MakeVerifyFileIntegrityReturn( int32_t value );
    void ExpectVerifyFileIntegrityIsNotCalled();

    MOCK_METHOD0( GetCloudUri, const std::string&() );
    void MakeGetCloudUriReturn( const std::string& value );
    void ExpectGetCloudUriIsNotCalled();

    MOCK_METHOD0( GetCloudInterval, uint32_t() );
    void MakeGetCloudIntervalReturn( uint32_t value );
    void ExpectGetCloudIntervalIsNotCalled();

    MOCK_METHOD0( GetSupportedComponentList, const std::vector<PmComponent>&() );
    void MakeGetSupportedComponentListReturn( const std::vector<PmComponent>& value );
    void ExpectGetSupportedComponentListIsNotCalled();

};

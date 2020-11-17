#pragma once
#include "MocksCommon.h"

#include "IPmConfig.h"
#include "PmTypes.h"

class MockPmConfig : public IPmConfig
{
public:
    MockPmConfig();
    ~MockPmConfig();

    MOCK_METHOD1( LoadBsConfig, int32_t( const std::string& ) );
    void MakeLoadBsConfigReturn( int32_t value );
    void ExpectLoadBsConfigIsNotCalled();

    MOCK_METHOD1( LoadPmConfig, int32_t( const std::string& ) );
    void MakeLoadPmConfigReturn( int32_t value );
    void ExpectLoadPmConfigIsNotCalled();

    MOCK_METHOD1( VerifyBsFileIntegrity, int32_t( const std::string& ) );
    void MakeVerifyBsFileIntegrityReturn( int32_t value );
    void ExpectVerifyBsFileIntegrityIsNotCalled();

    MOCK_METHOD1( VerifyPmFileIntegrity, int32_t( const std::string& ) );
    void MakeVerifyPmFileIntegrityReturn( int32_t value );
    void ExpectVerifyPmFileIntegrityIsNotCalled();

    MOCK_METHOD0( GetCloudUri, const std::string&() );
    void MakeGetCloudUriReturn( const std::string& value );
    void ExpectGetCloudUriIsNotCalled();

    MOCK_METHOD0( GetCloudInterval, uint32_t() );
    void MakeGetCloudIntervalReturn( uint32_t value );
    void ExpectGetCloudIntervalIsNotCalled();

    MOCK_METHOD0( GetLogLevel, uint32_t() );
    void MakeGetLogLevelReturn( uint32_t value );
    void ExpectGetLogLevelIsNotCalled();

    MOCK_METHOD0( GetSupportedComponentList, const std::vector<PmComponent>&() );
    void MakeGetSupportedComponentListReturn( const std::vector<PmComponent>& value );
    void ExpectGetSupportedComponentListIsNotCalled();

};

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

    MOCK_METHOD1( PmConfigFileChanged, bool( const std::string& ) );
    void MakePmConfigFileChangedReturn( bool value );
    void ExpectPmConfigFileChangedIsNotCalled();

    MOCK_METHOD1( VerifyBsFileIntegrity, int32_t( const std::string& ) );
    void MakeVerifyBsFileIntegrityReturn( int32_t value );
    void ExpectVerifyBsFileIntegrityIsNotCalled();

    MOCK_METHOD1( VerifyPmFileIntegrity, int32_t( const std::string& ) );
    void MakeVerifyPmFileIntegrityReturn( int32_t value );
    void ExpectVerifyPmFileIntegrityIsNotCalled();

    MOCK_METHOD0( GetCloudIdentifyUri, const std::string&() );
    void MakeGetCloudIdentifyUriReturn( const std::string& value );
    void ExpectGetCloudIdentifyUriIsNotCalled();

    MOCK_METHOD0( GetCloudCheckinUri, const std::string&() );
    void MakeGetCloudCheckinUriReturn( const std::string& value );
    void ExpectGetCloudCheckinUriIsNotCalled();

    MOCK_METHOD0( GetCloudEventUri, const std::string& ( ) );
    void MakeGetCloudEventUriReturn( const std::string& value );
    void ExpectGetCloudEventUriIsNotCalled();

    MOCK_METHOD0( GetCloudCatalogUri, const std::string& ( ) );
    void MakeGetCloudCatalogUriReturn( const std::string& value );
    void ExpectGetCloudCatalogUriIsNotCalled();

    MOCK_METHOD0( GetCloudCheckinIntervalMs, uint32_t() );
    void MakeGetCloudCheckinIntervalMsReturn( uint32_t value );
    void ExpectGetCloudCheckinIntervalMsIsNotCalled();

    MOCK_METHOD0( GetLogLevel, uint32_t() );
    void MakeGetLogLevelReturn( uint32_t value );
    void ExpectGetLogLevelIsNotCalled();

    MOCK_METHOD0( GetSupportedComponentList, const std::vector<PmComponent>&() );
    void MakeGetSupportedComponentListReturn( const std::vector<PmComponent>& value );
    void ExpectGetSupportedComponentListIsNotCalled();

    MOCK_METHOD0( GetMaxFileCacheAge, uint32_t() );
    void MakeGetMaxFileCacheAgeReturn( uint32_t value );
    void ExpectGetMaxFileCacheAgeIsNotCalled();

private:
    std::string m_identifyUri;
    std::string m_checkinUri;
    std::string m_eventUri;
    std::string m_catalogUri;
};

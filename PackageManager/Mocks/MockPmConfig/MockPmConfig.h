#pragma once
#include "MocksCommon.h"

#include "IPmConfig.h"

class MockPmConfig : public IPmConfig
{
public:
    MockPmConfig();
    ~MockPmConfig();

    MOCK_METHOD1( LoadPmConfig, int32_t( const std::string& ) );
    void MakeLoadPmConfigReturn( int32_t value );
    void ExpectLoadPmConfigIsNotCalled();

    MOCK_METHOD1( PmConfigFileChanged, bool( const std::string& ) );
    void MakePmConfigFileChangedReturn( bool value );
    void ExpectPmConfigFileChangedIsNotCalled();

    MOCK_METHOD1( VerifyPmFileIntegrity, int32_t( const std::string& ) );
    void MakeVerifyPmFileIntegrityReturn( int32_t value );
    void ExpectVerifyPmFileIntegrityIsNotCalled();

    MOCK_METHOD0( GetCloudIdentifyUri, const std::string&() );
    void MakeGetCloudIdentifyUriReturn( const std::string& value );
    void ExpectGetCloudIdentifyUriIsNotCalled();

    MOCK_METHOD0( GetCloudCheckinUri, std::string() );
    void MakeGetCloudCheckinUriReturn( std::string value );
    void ExpectGetCloudCheckinUriIsNotCalled();

    MOCK_METHOD0( GetCloudEventUri, std::string ( ) );
    void MakeGetCloudEventUriReturn( std::string value );
    void ExpectGetCloudEventUriIsNotCalled();

    MOCK_METHOD0( GetCloudCatalogUri, std::string ( ) );
    void MakeGetCloudCatalogUriReturn( std::string value );
    void ExpectGetCloudCatalogUriIsNotCalled();

    MOCK_METHOD0( GetCloudCheckinIntervalMs, uint32_t() );
    void MakeGetCloudCheckinIntervalMsReturn( uint32_t value );
    void ExpectGetCloudCheckinIntervalMsIsNotCalled();

    MOCK_METHOD0( GetLogLevel, uint32_t() );
    void MakeGetLogLevelReturn( uint32_t value );
    void ExpectGetLogLevelIsNotCalled();

    MOCK_METHOD0( GetMaxFileCacheAgeS, uint32_t() );
    void MakeGetMaxFileCacheAgeSReturn( uint32_t value );
    void ExpectGetMaxFileCacheAgeSIsNotCalled();

    MOCK_METHOD0( GetMaxEventTtlS, uint32_t() );
    void MakeGetMaxEventTtlSReturn( uint32_t value );
    void ExpectGetMaxEventTtlSIsNotCalled();

    MOCK_METHOD0( AllowPostInstallReboots, bool() );
    void MakeAllowPostInstallRebootsReturn( bool value );

    MOCK_METHOD0( GetRebootThrottleS, uint32_t() );
    void MakeGetRebootThrottleSReturn( uint32_t value );
    void ExpectGetRebootThrottleSNotCalled();

    MOCK_METHOD0( GetWatchdogTimeoutMs, uint32_t() );
    void MakeGetWatchdogTimeoutMsReturn( uint32_t value );
    void ExpectGetWatchdogTimeoutMsNotCalled();

    MOCK_METHOD0( GetNetworkFailureRetryInterval, uint32_t() );
    void MakeGetNetworkFailureRetryIntervalReturn( uint32_t value );
    void ExpectGetNetworkFailureRetryIntervalNotCalled();

private:
    std::string m_identifyUri;
    std::string m_checkinUri;
    std::string m_eventUri;
    std::string m_catalogUri;
};

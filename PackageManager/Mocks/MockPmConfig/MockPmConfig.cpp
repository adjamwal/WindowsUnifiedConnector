#include "MockPmConfig.h"

MockPmConfig::MockPmConfig()
{
    MakeLoadBsConfigReturn( int32_t() );
    MakeLoadPmConfigReturn( int32_t() );
    MakeVerifyBsFileIntegrityReturn( int32_t() );
    MakeVerifyPmFileIntegrityReturn( int32_t() );
    MakeGetCloudIdentifyUriReturn( "" );
    MakeGetCloudCheckinUriReturn( "" );
    MakeGetCloudEventUriReturn( "" );
    MakeGetCloudCatalogUriReturn( "" );
    MakeGetCloudCheckinIntervalMsReturn( uint32_t() );
    MakeAllowPostInstallRebootsReturn( false );
}

MockPmConfig::~MockPmConfig()
{
}

void MockPmConfig::MakeLoadBsConfigReturn( int32_t value )
{
    ON_CALL( *this, LoadBsConfig( _ ) ).WillByDefault( Return( value ) );
}

void MockPmConfig::MakeLoadPmConfigReturn( int32_t value )
{
    ON_CALL( *this, LoadPmConfig( _ ) ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectLoadBsConfigIsNotCalled()
{
    EXPECT_CALL( *this, LoadBsConfig( _ ) ).Times( 0 );
}

void MockPmConfig::ExpectLoadPmConfigIsNotCalled()
{
    EXPECT_CALL( *this, LoadPmConfig( _ ) ).Times( 0 );
}

void MockPmConfig::MakePmConfigFileChangedReturn( bool value )
{
    ON_CALL( *this, PmConfigFileChanged( _ ) ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectPmConfigFileChangedIsNotCalled()
{
    EXPECT_CALL( *this, PmConfigFileChanged( _ ) ).Times( 0 );
}

void MockPmConfig::MakeVerifyBsFileIntegrityReturn( int32_t value )
{
    ON_CALL( *this, VerifyBsFileIntegrity( _ ) ).WillByDefault( Return( value ) );
}

void MockPmConfig::MakeVerifyPmFileIntegrityReturn( int32_t value )
{
    ON_CALL( *this, VerifyPmFileIntegrity( _ ) ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectVerifyBsFileIntegrityIsNotCalled()
{
    EXPECT_CALL( *this, VerifyBsFileIntegrity( _ ) ).Times( 0 );
}

void MockPmConfig::ExpectVerifyPmFileIntegrityIsNotCalled()
{
    EXPECT_CALL( *this, VerifyPmFileIntegrity( _ ) ).Times( 0 );
}

void MockPmConfig::MakeGetCloudIdentifyUriReturn( const std::string& value )
{
    m_identifyUri = value;
    ON_CALL( *this, GetCloudIdentifyUri() ).WillByDefault( ReturnRef( m_identifyUri ) );
}

void MockPmConfig::ExpectGetCloudIdentifyUriIsNotCalled()
{
    EXPECT_CALL( *this, GetCloudIdentifyUri() ).Times( 0 );
}

void MockPmConfig::MakeGetCloudCheckinUriReturn( const std::string& value )
{
    m_checkinUri = value;
    ON_CALL( *this, GetCloudCheckinUri() ).WillByDefault( ReturnRef( m_checkinUri ) );
}

void MockPmConfig::ExpectGetCloudCheckinUriIsNotCalled()
{
    EXPECT_CALL( *this, GetCloudCheckinUri() ).Times( 0 );
}

void MockPmConfig::MakeGetCloudEventUriReturn( const std::string& value )
{
    m_eventUri = value;
    ON_CALL( *this, GetCloudEventUri() ).WillByDefault( ReturnRef( m_eventUri ) );
}

void MockPmConfig::ExpectGetCloudEventUriIsNotCalled()
{
    EXPECT_CALL( *this, GetCloudEventUri() ).Times( 0 );
}

void MockPmConfig::MakeGetCloudCatalogUriReturn( const std::string& value )
{
    m_catalogUri = value;
    ON_CALL( *this, GetCloudCatalogUri() ).WillByDefault( ReturnRef( m_catalogUri ) );
}

void MockPmConfig::ExpectGetCloudCatalogUriIsNotCalled()
{
    EXPECT_CALL( *this, GetCloudCatalogUri() ).Times( 0 );
}

void MockPmConfig::MakeGetCloudCheckinIntervalMsReturn( uint32_t value )
{
    ON_CALL( *this, GetCloudCheckinIntervalMs() ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectGetCloudCheckinIntervalMsIsNotCalled()
{
    EXPECT_CALL( *this, GetCloudCheckinIntervalMs() ).Times( 0 );
}

void MockPmConfig::MakeGetLogLevelReturn( uint32_t value )
{
    ON_CALL( *this, GetLogLevel() ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectGetLogLevelIsNotCalled()
{
    EXPECT_CALL( *this, GetLogLevel() ).Times( 0 );
}

void MockPmConfig::MakeGetMaxFileCacheAgeSReturn( uint32_t value )
{
    ON_CALL( *this, GetMaxFileCacheAgeS() ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectGetMaxFileCacheAgeSIsNotCalled()
{
    EXPECT_CALL( *this, GetMaxFileCacheAgeS() ).Times( 0 );
}

void MockPmConfig::MakeGetMaxEventTtlSReturn( uint32_t value )
{
    ON_CALL( *this, GetMaxEventTtlS() ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectGetMaxEventTtlSIsNotCalled()
{
    EXPECT_CALL( *this, GetMaxEventTtlS() ).Times( 0 );
}

void MockPmConfig::MakeAllowPostInstallRebootsReturn( bool value )
{
    ON_CALL( *this, AllowPostInstallReboots() ).WillByDefault( Return( value ) );
}

void MockPmConfig::MakeGetRebootThrottleSReturn( uint32_t value )
{
    ON_CALL( *this, GetRebootThrottleS() ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectGetRebootThrottleSNotCalled()
{
    EXPECT_CALL( *this, GetRebootThrottleS() ).Times( 0 );
}

void MockPmConfig::MakeGetWatchdogTimeoutMsReturn( uint32_t value )
{
    ON_CALL( *this, GetWatchdogTimeoutMs() ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectGetWatchdogTimeoutMsNotCalled()
{
    EXPECT_CALL( *this, GetWatchdogTimeoutMs() ).Times( 0 );
}
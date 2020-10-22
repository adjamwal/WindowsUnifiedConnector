#include "MockPmConfig.h"

MockPmConfig::MockPmConfig()
{
    MakeLoadReturn( int32_t() );
    MakeVerifyFileIntegrityReturn( int32_t() );
    MakeGetCloudUriReturn( const std::string&() );
    MakeGetCloudIntervalReturn( uint32_t() );
    MakeGetSupportedComponentListReturn( const std::vector<PmComponent>&() );
}

MockPmConfig::~MockPmConfig()
{
}

void MockPmConfig::MakeLoadReturn( int32_t value )
{
    ON_CALL( *this, Load( _ ) ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectLoadIsNotCalled()
{
    EXPECT_CALL( *this, Load( _ ) ).Times( 0 );
}

void MockPmConfig::MakeVerifyFileIntegrityReturn( int32_t value )
{
    ON_CALL( *this, VerifyFileIntegrity( _ ) ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectVerifyFileIntegrityIsNotCalled()
{
    EXPECT_CALL( *this, VerifyFileIntegrity( _ ) ).Times( 0 );
}

void MockPmConfig::MakeGetCloudUriReturn( const std::string& value )
{
    ON_CALL( *this, GetCloudUri() ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectGetCloudUriIsNotCalled()
{
    EXPECT_CALL( *this, GetCloudUri() ).Times( 0 );
}

void MockPmConfig::MakeGetCloudIntervalReturn( uint32_t value )
{
    ON_CALL( *this, GetCloudInterval() ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectGetCloudIntervalIsNotCalled()
{
    EXPECT_CALL( *this, GetCloudInterval() ).Times( 0 );
}

void MockPmConfig::MakeGetSupportedComponentListReturn( const std::vector<PmComponent>& value )
{
    ON_CALL( *this, GetSupportedComponentList() ).WillByDefault( Return( value ) );
}

void MockPmConfig::ExpectGetSupportedComponentListIsNotCalled()
{
    EXPECT_CALL( *this, GetSupportedComponentList() ).Times( 0 );
}


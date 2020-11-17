#include "MockPmConfig.h"

MockPmConfig::MockPmConfig()
{
    MakeLoadBsConfigReturn( int32_t() );
    MakeLoadPmConfigReturn( int32_t() );
    MakeVerifyBsFileIntegrityReturn( int32_t() );
    MakeVerifyPmFileIntegrityReturn( int32_t() );
    MakeGetCloudUriReturn( "" );
    MakeGetCloudIntervalReturn( uint32_t() );
    MakeGetSupportedComponentListReturn( {} );
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

void MockPmConfig::MakeGetCloudUriReturn( const std::string& value )
{
    ON_CALL( *this, GetCloudUri() ).WillByDefault( ReturnRef( value ) );
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
    ON_CALL( *this, GetSupportedComponentList() ).WillByDefault( ReturnRef( value ) );
}

void MockPmConfig::ExpectGetSupportedComponentListIsNotCalled()
{
    EXPECT_CALL( *this, GetSupportedComponentList() ).Times( 0 );
}


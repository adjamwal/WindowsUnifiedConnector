#include "pch.h"
#include "MockCloudEventBuilder.h"

MockCloudEventBuilder::MockCloudEventBuilder()
{
}

MockCloudEventBuilder::~MockCloudEventBuilder()
{
}

void MockCloudEventBuilder::MakeBuildReturn( std::string value )
{
    ON_CALL( *this, Build() ).WillByDefault( Return( value ) );
}

void MockCloudEventBuilder::ExpectBuildIsNotCalled()
{
    EXPECT_CALL( *this, Build() ).Times( 0 );
}

void MockCloudEventBuilder::ExpectResetIsNotCalled()
{
    EXPECT_CALL( *this, Reset() ).Times( 0 );
}

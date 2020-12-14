#include "MockPackageConfigProcessor.h"

MockPackageConfigProcessor::MockPackageConfigProcessor()
{
    MakeProcessConfigReturn( false );
}

MockPackageConfigProcessor::~MockPackageConfigProcessor()
{

}

void MockPackageConfigProcessor::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockPackageConfigProcessor::MakeProcessConfigReturn( bool value )
{
    ON_CALL( *this, ProcessConfig( _ ) ).WillByDefault( Return( value ) );
}

void MockPackageConfigProcessor::ExpectProcessConfigIsNotCalled()
{
    EXPECT_CALL( *this, ProcessConfig( _ ) ).Times( 0 );
}
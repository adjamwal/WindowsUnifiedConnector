#include "MockPackageConfigProcessor.h"

MockPackageConfigProcessor::MockPackageConfigProcessor()
{

}

MockPackageConfigProcessor::~MockPackageConfigProcessor()
{

}

void MockPackageConfigProcessor::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockPackageConfigProcessor::MakeProcessComponentPackageReturn( bool value )
{
    ON_CALL( *this, ProcessConfig( _ ) ).WillByDefault( Return( value ) );
}

void MockPackageConfigProcessor::ExpectProcessComponentPackageIsNotCalled()
{
    EXPECT_CALL( *this, ProcessConfig( _ ) ).Times( 0 );
}
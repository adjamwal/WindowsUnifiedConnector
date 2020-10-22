#include "MockComponentPackageProcessor.h"

MockComponentPackageProcessor::MockComponentPackageProcessor()
{
    MakeProcessComponentPackageReturn( false );
}

MockComponentPackageProcessor::~MockComponentPackageProcessor()
{
}

void MockComponentPackageProcessor::MakeProcessComponentPackageReturn( bool value )
{
    ON_CALL( *this, ProcessComponentPackage( _ ) ).WillByDefault( Return( value ) );
}

void MockComponentPackageProcessor::ExpectProcessComponentPackageIsNotCalled()
{
    EXPECT_CALL( *this, ProcessComponentPackage( _ ) ).Times( 0 );
}


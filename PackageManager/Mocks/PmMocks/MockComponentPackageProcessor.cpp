#include "MockComponentPackageProcessor.h"

MockComponentPackageProcessor::MockComponentPackageProcessor()
{
    MakeProcessComponentPackageReturn( false );
}

MockComponentPackageProcessor::~MockComponentPackageProcessor()
{
}

void MockComponentPackageProcessor::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockComponentPackageProcessor::MakeProcessComponentPackageReturn( bool value )
{
    ON_CALL( *this, ProcessPackageBinaries( _ ) ).WillByDefault( Return( value ) );
}

void MockComponentPackageProcessor::ExpectProcessComponentPackageIsNotCalled()
{
    EXPECT_CALL( *this, ProcessPackageBinaries( _ ) ).Times( 0 );
}


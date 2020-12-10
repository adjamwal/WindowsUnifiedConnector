#include "MockComponentPackageProcessor.h"

MockComponentPackageProcessor::MockComponentPackageProcessor()
{
    MakeIsActionableReturn( false );
    MakeHasConfigsReturn( false );
    MakeProcessPackageBinariesReturn( false );
    MakeProcessConfigsForPackageReturn( false );
}

MockComponentPackageProcessor::~MockComponentPackageProcessor()
{
}

void MockComponentPackageProcessor::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockComponentPackageProcessor::MakeIsActionableReturn( bool value )
{
    ON_CALL( *this, IsActionable( _ ) ).WillByDefault( Return( value ) );
}

void MockComponentPackageProcessor::ExpectIsActionableIsNotCalled()
{
    EXPECT_CALL( *this, IsActionable( _ ) ).Times( 0 );
}

void MockComponentPackageProcessor::MakeHasConfigsReturn( bool value )
{
    ON_CALL( *this, HasConfigs( _ ) ).WillByDefault( Return( value ) );
}

void MockComponentPackageProcessor::ExpectHasConfigsIsNotCalled()
{
    EXPECT_CALL( *this, HasConfigs( _ ) ).Times( 0 );
}

void MockComponentPackageProcessor::MakeProcessPackageBinariesReturn( bool value )
{
    ON_CALL( *this, ProcessPackageBinaries( _ ) ).WillByDefault( Return( value ) );
}

void MockComponentPackageProcessor::ExpectProcessPackageBinariesIsNotCalled()
{
    EXPECT_CALL( *this, ProcessPackageBinaries( _ ) ).Times( 0 );
}

void MockComponentPackageProcessor::MakeProcessConfigsForPackageReturn( bool value )
{
    ON_CALL( *this, ProcessConfigsForPackage( _ ) ).WillByDefault( Return( value ) );
}

void MockComponentPackageProcessor::ExpectProcessConfigsForPackageIsNotCalled()
{
    EXPECT_CALL( *this, ProcessConfigsForPackage( _ ) ).Times( 0 );
}

#include "MockComponentPackageProcessor.h"

MockComponentPackageProcessor::MockComponentPackageProcessor()
{
    MakeHasDownloadedBinaryReturn( false );
    MakeHasConfigsReturn( false );
    MakeDownloadPackageBinaryReturn( false );
    MakeProcessPackageBinaryReturn( false );
    MakeProcessConfigsForPackageReturn( false );
}

MockComponentPackageProcessor::~MockComponentPackageProcessor()
{
}

void MockComponentPackageProcessor::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockComponentPackageProcessor::MakeHasDownloadedBinaryReturn( bool value )
{
    ON_CALL( *this, PreDownloadedBinaryExists( _ ) ).WillByDefault( Return( value ) );
}

void MockComponentPackageProcessor::ExpectHasDownloadedBinaryIsNotCalled()
{
    EXPECT_CALL( *this, PreDownloadedBinaryExists( _ ) ).Times( 0 );
}

void MockComponentPackageProcessor::MakeHasConfigsReturn( bool value )
{
    ON_CALL( *this, HasConfigs( _ ) ).WillByDefault( Return( value ) );
}

void MockComponentPackageProcessor::ExpectHasConfigsIsNotCalled()
{
    EXPECT_CALL( *this, HasConfigs( _ ) ).Times( 0 );
}

void MockComponentPackageProcessor::MakeDownloadPackageBinaryReturn( bool value )
{
    ON_CALL( *this, DownloadPackageBinary( _ ) ).WillByDefault( Return( value ) );
}

void MockComponentPackageProcessor::ExpectDownloadPackageBinaryIsNotCalled()
{
    EXPECT_CALL( *this, DownloadPackageBinary( _ ) ).Times( 0 );
}

void MockComponentPackageProcessor::MakeProcessPackageBinaryReturn( bool value )
{
    ON_CALL( *this, ProcessPackageBinary( _ ) ).WillByDefault( Return( value ) );
}

void MockComponentPackageProcessor::ExpectProcessPackageBinaryIsNotCalled()
{
    EXPECT_CALL( *this, ProcessPackageBinary( _ ) ).Times( 0 );
}

void MockComponentPackageProcessor::MakeProcessConfigsForPackageReturn( bool value )
{
    ON_CALL( *this, ProcessConfigsForPackage( _ ) ).WillByDefault( Return( value ) );
}

void MockComponentPackageProcessor::ExpectProcessConfigsForPackageIsNotCalled()
{
    EXPECT_CALL( *this, ProcessConfigsForPackage( _ ) ).Times( 0 );
}

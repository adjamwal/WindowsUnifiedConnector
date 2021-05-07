#include "MockManifestProcessor.h"

MockManifestProcessor::MockManifestProcessor()
{
    MakeProcessManifestReturn( false );
}

MockManifestProcessor::~MockManifestProcessor()
{
}

void MockManifestProcessor::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockManifestProcessor::MakeProcessManifestReturn( bool value )
{
    ON_CALL( *this, ProcessManifest( _, _ ) ).WillByDefault( Return( value ) );
}

void MockManifestProcessor::ExpectProcessManifestIsNotCalled()
{
    EXPECT_CALL( *this, ProcessManifest( _, _ ) ).Times( 0 );
}


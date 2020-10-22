#include "MockManifestProcessor.h"

MockManifestProcessor::MockManifestProcessor()
{
    MakeProcessManifestReturn( false );
}

MockManifestProcessor::~MockManifestProcessor()
{
}

void MockManifestProcessor::MakeProcessManifestReturn( bool value )
{
    ON_CALL( *this, ProcessManifest( _ ) ).WillByDefault( Return( value ) );
}

void MockManifestProcessor::ExpectProcessManifestIsNotCalled()
{
    EXPECT_CALL( *this, ProcessManifest( _ ) ).Times( 0 );
}


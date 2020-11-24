#include "MockCheckinManifestRetriever.h"

MockCheckinManifestRetriever::MockCheckinManifestRetriever()
{
    MakeGetCheckinManifestFromReturn( std::string() );
}

MockCheckinManifestRetriever::~MockCheckinManifestRetriever()
{
}

void MockCheckinManifestRetriever::MakeGetCheckinManifestFromReturn( std::string value )
{
    ON_CALL( *this, GetCheckinManifestFrom( _, _ ) ).WillByDefault( Return( value ) );
}

void MockCheckinManifestRetriever::ExpectGetCheckinManifestFromIsNotCalled()
{
    EXPECT_CALL( *this, GetCheckinManifestFrom( _, _ ) ).Times( 0 );
}


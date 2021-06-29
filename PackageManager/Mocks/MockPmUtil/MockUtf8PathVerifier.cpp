#include "MockUtf8PathVerifier.h"

MockUtf8PathVerifier::MockUtf8PathVerifier()
{

}

MockUtf8PathVerifier::~MockUtf8PathVerifier()
{

}

void MockUtf8PathVerifier::MakeIsPathValidReturn( bool value )
{
    ON_CALL( *this, IsPathValid( _ ) ).WillByDefault( Return( value ) );
}

void MockUtf8PathVerifier::ExpectIsPathValidNotCalled()
{
    EXPECT_CALL( *this, IsPathValid( _ ) ).Times( 0 );
}

void MockUtf8PathVerifier::ExpectPruneInvalidPathsFromListIsNotCalled()
{
    EXPECT_CALL( *this, PruneInvalidPathsFromList( _ ) ).Times( 0 );
}
#include "MockPmManifest.h"
#include "PmTypes.h"

MockPmManifest::MockPmManifest()
{
    MakeParseManifestReturn( int32_t() );
    MakeGetPackageListReturn( {} );
}

MockPmManifest::~MockPmManifest()
{
}

void MockPmManifest::MakeParseManifestReturn( int32_t value )
{
    ON_CALL( *this, ParseManifest( _ ) ).WillByDefault( Return( value ) );
}

void MockPmManifest::ExpectParseManifestIsNotCalled()
{
    EXPECT_CALL( *this, ParseManifest( _ ) ).Times( 0 );
}

void MockPmManifest::ExpectSetDeploymentIdIsNotCalled()
{
    EXPECT_CALL( *this, SetDeploymentId( _ ) ).Times( 0 );
}

void MockPmManifest::MakeGetDeploymentIdReturn( std::string value )
{
    ON_CALL( *this, GetDeploymentId() ).WillByDefault( ReturnRef( value ) );
}

void MockPmManifest::ExpectGetDeploymentIdIsNotCalled()
{
    EXPECT_CALL( *this, GetDeploymentId() ).Times( 0 );
}

void MockPmManifest::MakeGetPackageListReturn( std::vector<PmComponent> value )
{
    ON_CALL( *this, GetPackageList() ).WillByDefault( Return( value ) );
}

void MockPmManifest::ExpectGetPackageListIsNotCalled()
{
    EXPECT_CALL( *this, GetPackageList() ).Times( 0 );
}

void MockPmManifest::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

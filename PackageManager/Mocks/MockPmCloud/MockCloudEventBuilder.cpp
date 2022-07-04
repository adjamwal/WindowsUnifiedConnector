#include "pch.h"
#include "MockCloudEventBuilder.h"

MockCloudEventBuilder::MockCloudEventBuilder()
{
    ON_CALL( *this, FromJson( _ ) ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, WithUCID( _ ) ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, WithType( _ ) ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, WithPackageID( _ ) ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, WithPackage( _, _ ) ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, WithError( _, _ ) ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, WithSubError( _, _ ) ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, WithOldFile( _, _, _ ) ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, WithNewFile( _, _, _ ) ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, WithFrom( _ ) ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, WithTse( _ ) ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, WithTseNow() ).WillByDefault( ReturnRef( *this ) );
    ON_CALL( *this, GetPackageName() ).WillByDefault( Return( "" ) );
    ON_CALL( *this, GetPackageVersion() ).WillByDefault( Return( "" ) );
    ON_CALL( *this, GetRFC3339Tse() ).WillByDefault( Return( "" ) );
    MakeBuildReturn( "" );
}

MockCloudEventBuilder::~MockCloudEventBuilder()
{
}

void MockCloudEventBuilder::MakeBuildReturn( std::string value )
{
    ON_CALL( *this, Build() ).WillByDefault( Return( value ) );
}

void MockCloudEventBuilder::ExpectBuildIsNotCalled()
{
    EXPECT_CALL( *this, Build() ).Times( 0 );
}

void MockCloudEventBuilder::ExpectResetIsNotCalled()
{
    EXPECT_CALL( *this, Reset() ).Times( 0 );
}

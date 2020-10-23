#include "MockCertsAdapter.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformConfiguration.h"

MockCertsAdapter::MockCertsAdapter()
{
    MakeGetCertsListReturn( PmHttpCertList() );
}

MockCertsAdapter::~MockCertsAdapter()
{
}

void MockCertsAdapter::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockCertsAdapter::MakeGetCertsListReturn( PmHttpCertList value )
{
    ON_CALL( *this, GetCertsList() ).WillByDefault( Return( value ) );
}

void MockCertsAdapter::ExpectGetCertsListIsNotCalled()
{
    EXPECT_CALL( *this, GetCertsList() ).Times( 0 );
}


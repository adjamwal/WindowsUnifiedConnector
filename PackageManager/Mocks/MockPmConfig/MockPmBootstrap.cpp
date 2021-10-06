#include "MockPmBootstrap.h"

MockPmBootstrap::MockPmBootstrap()
{
    MakeGetIdentifyUriReturn( "" );
}

MockPmBootstrap::~MockPmBootstrap()
{

}

void MockPmBootstrap::MakeLoadPmBootstrapReturn( int32_t value )
{
    ON_CALL( *this, LoadPmBootstrap( _ ) ).WillByDefault( Return( value ) );
}

void MockPmBootstrap::ExpectLoadPmBootstrapIsNotCalled()
{
    EXPECT_CALL( *this, LoadPmBootstrap( _ ) ).Times( 0 );
}

void MockPmBootstrap::MakeGetIdentifyUriReturn( std::string value )
{
    ON_CALL( *this, GetIdentifyUri() ).WillByDefault( Return( value ) );
}

void MockPmBootstrap::ExpectGetIdentifyUriIsNotCalled()
{
    EXPECT_CALL( *this, GetIdentifyUri() ).Times( 0 );
}
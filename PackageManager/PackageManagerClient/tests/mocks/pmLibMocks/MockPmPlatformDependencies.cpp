#include "MockPmPlatformDependencies.h"

MockPmPlatformDependencies::MockPmPlatformDependencies()
{
    MakeConfigurationReturn( IPmPlatformConfiguration&() );
    MakeComponentManagerReturn( IPmPlatformComponentManager&() );
}

MockPmPlatformDependencies::~MockPmPlatformDependencies()
{
}

void MockPmPlatformDependencies::MakeConfigurationReturn( IPmPlatformConfiguration& value )
{
    ON_CALL( *this, Configuration() ).WillByDefault( Return( value ) );
}

void MockPmPlatformDependencies::ExpectConfigurationIsNotCalled()
{
    EXPECT_CALL( *this, Configuration() ).Times( 0 );
}

void MockPmPlatformDependencies::MakeComponentManagerReturn( IPmPlatformComponentManager& value )
{
    ON_CALL( *this, ComponentManager() ).WillByDefault( Return( value ) );
}

void MockPmPlatformDependencies::ExpectComponentManagerIsNotCalled()
{
    EXPECT_CALL( *this, ComponentManager() ).Times( 0 );
}


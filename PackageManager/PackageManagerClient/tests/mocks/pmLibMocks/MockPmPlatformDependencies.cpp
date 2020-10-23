#include "MockPmPlatformDependencies.h"

MockPmPlatformDependencies::MockPmPlatformDependencies()
{
}

MockPmPlatformDependencies::~MockPmPlatformDependencies()
{
}

void MockPmPlatformDependencies::MakeConfigurationReturn( IPmPlatformConfiguration& value )
{
    ON_CALL( *this, Configuration() ).WillByDefault( ReturnRef( value ) );
}

void MockPmPlatformDependencies::ExpectConfigurationIsNotCalled()
{
    EXPECT_CALL( *this, Configuration() ).Times( 0 );
}

void MockPmPlatformDependencies::MakeComponentManagerReturn( IPmPlatformComponentManager& value )
{
    ON_CALL( *this, ComponentManager() ).WillByDefault( ReturnRef( value ) );
}

void MockPmPlatformDependencies::ExpectComponentManagerIsNotCalled()
{
    EXPECT_CALL( *this, ComponentManager() ).Times( 0 );
}


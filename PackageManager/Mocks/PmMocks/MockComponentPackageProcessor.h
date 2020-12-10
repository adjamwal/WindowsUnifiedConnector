#pragma once
#include "MocksCommon.h"

#include "IComponentPackageProcessor.h"
#include "IPmPlatformDependencies.h"
#include "PmTypes.h"

class MockComponentPackageProcessor : public IComponentPackageProcessor
{
public:
    MockComponentPackageProcessor();
    ~MockComponentPackageProcessor();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* ) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD1( IsActionable, bool( PmComponent& ) );
    void MakeIsActionableReturn( bool value );
    void ExpectIsActionableIsNotCalled();

    MOCK_METHOD1( HasConfigs, bool( PmComponent& ) );
    void MakeHasConfigsReturn( bool value );
    void ExpectHasConfigsIsNotCalled();

    MOCK_METHOD1( ProcessPackageBinaries, bool( PmComponent& ) );
    void MakeProcessPackageBinariesReturn( bool value );
    void ExpectProcessPackageBinariesIsNotCalled();

    MOCK_METHOD1( ProcessConfigsForPackage, bool( PmComponent& ) );
    void MakeProcessConfigsForPackageReturn( bool value );
    void ExpectProcessConfigsForPackageIsNotCalled();
};

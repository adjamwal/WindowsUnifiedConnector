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

    MOCK_METHOD1( ProcessComponentPackage, bool( PmComponent& ) );
    void MakeProcessComponentPackageReturn( bool value );
    void ExpectProcessComponentPackageIsNotCalled();

};

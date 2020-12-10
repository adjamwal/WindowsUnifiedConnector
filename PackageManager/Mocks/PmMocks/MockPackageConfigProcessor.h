#pragma once
#include "MocksCommon.h"

#include "IPackageConfigProcessor.h"
#include "IPmPlatformDependencies.h"
#include "PmTypes.h"

class MockPackageConfigProcessor : public IPackageConfigProcessor
{
public:
    MockPackageConfigProcessor();
    ~MockPackageConfigProcessor();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* ) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD1( ProcessConfig, bool( PackageConfigInfo& ) );
    void MakeProcessConfigReturn( bool value );
    void ExpectProcessConfigIsNotCalled();

};

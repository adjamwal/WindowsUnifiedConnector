#pragma once
#include "MocksCommon.h"

#include "IComponentPackageProcessor.h"
#include "IPmPlatformDependencies.h"
#include "PackageException.h"
#include "PmTypes.h"

class MockComponentPackageProcessor : public IComponentPackageProcessor
{
public:
    MockComponentPackageProcessor();
    ~MockComponentPackageProcessor();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* ) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD1( PreDownloadedBinaryExists, bool( PmComponent& ) );
    void MakePreDownloadedBinaryExistsReturn( bool value );
    void ExpectPreDownloadedBinaryExistsIsNotCalled();

    MOCK_METHOD1( HasConfigs, bool( PmComponent& ) );
    void MakeHasConfigsReturn( bool value );
    void ExpectHasConfigsIsNotCalled();

    MOCK_METHOD1( DownloadPackageBinary, bool( PmComponent& ) );
    void MakeDownloadPackageBinaryReturn( bool value );
    void ExpectDownloadPackageBinaryIsNotCalled();

    MOCK_METHOD1( ProcessPackageBinary, bool( PmComponent& ) );
    void MakeProcessPackageBinaryReturn( bool value );
    void ExpectProcessPackageBinaryIsNotCalled();

    MOCK_METHOD1( ProcessConfigsForPackage, bool( PmComponent& ) );
    void MakeProcessConfigsForPackageReturn( bool value );
    void ExpectProcessConfigsForPackageIsNotCalled();
};

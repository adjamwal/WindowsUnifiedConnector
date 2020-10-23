#pragma once
#include "MocksCommon.h"

#include "IManifestProcessor.h"
#include "IPmPlatformDependencies.h"

class MockManifestProcessor : public IManifestProcessor
{
public:
    MockManifestProcessor();
    ~MockManifestProcessor();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* ) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD1( ProcessManifest, bool( std::string ) );
    void MakeProcessManifestReturn( bool value );
    void ExpectProcessManifestIsNotCalled();

};

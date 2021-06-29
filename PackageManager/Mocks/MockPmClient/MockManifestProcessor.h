#pragma once
#include "MocksCommon.h"

#include "IManifestProcessor.h"
#include "IPmPlatformDependencies.h"
#include "PmTypes.h"
#include <vector>

class MockManifestProcessor : public IManifestProcessor
{
public:
    MockManifestProcessor();
    ~MockManifestProcessor();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* ) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD2( ProcessManifest, bool( std::string, bool& ) );
    void MakeProcessManifestReturn( bool value );
    void ExpectProcessManifestIsNotCalled();
};

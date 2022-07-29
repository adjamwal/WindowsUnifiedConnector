#pragma once
#include "MocksCommon.h"

#include "IPmManifest.h"

class MockPmManifest : public IPmManifest
{
public:
    MockPmManifest();
    ~MockPmManifest();

    MOCK_METHOD1( ParseManifest, int32_t( const std::string& ) );
    void MakeParseManifestReturn( int32_t value );
    void ExpectParseManifestIsNotCalled();

    MOCK_METHOD1( SetDeploymentId, void( const std::string& ) );
    void ExpectSetDeploymentIdIsNotCalled();

    MOCK_METHOD0( GetDeploymentId, const std::string& () );
    void MakeGetDeploymentIdReturn( std::string value );
    void ExpectGetDeploymentIdIsNotCalled();

    MOCK_METHOD0( GetPackageList, std::vector<PmComponent>() );
    void MakeGetPackageListReturn( std::vector<PmComponent> value );
    void ExpectGetPackageListIsNotCalled();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* ) );
    void ExpectInitializeIsNotCalled();
};

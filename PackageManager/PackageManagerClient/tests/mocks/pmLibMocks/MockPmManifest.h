#pragma once
#include "MocksCommon.h"

#include "IPmManifest.h"

class MockPmManifest : public IPmManifest
{
public:
    MockPmManifest();
    ~MockPmManifest();

    MOCK_METHOD1( ParseManifest, int32_t( std::string& ) );
    void MakeParseManifestReturn( int32_t value );
    void ExpectParseManifestIsNotCalled();

    MOCK_METHOD0( GetPackageList, std::vector<PmComponent>() );
    void MakeGetPackageListReturn( std::vector<PmComponent> value );
    void ExpectGetPackageListIsNotCalled();

};

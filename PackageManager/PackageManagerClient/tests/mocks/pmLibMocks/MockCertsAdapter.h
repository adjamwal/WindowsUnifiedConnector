#pragma once
#include "MocksCommon.h"

#include "ICertsAdapter.h"

class MockCertsAdapter : public ICertsAdapter
{
public:
    MockCertsAdapter();
    ~MockCertsAdapter();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* ) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD0( GetCertsList, PmHttpCertList() );
    void MakeGetCertsListReturn( PmHttpCertList value );
    void ExpectGetCertsListIsNotCalled();

};

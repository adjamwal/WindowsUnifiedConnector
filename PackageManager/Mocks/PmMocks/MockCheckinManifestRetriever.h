#pragma once
#include "MocksCommon.h"

#include "ICheckinManifestRetriever.h"

class MockCheckinManifestRetriever : public ICheckinManifestRetriever
{
public:
    MockCheckinManifestRetriever();
    ~MockCheckinManifestRetriever();

    MOCK_METHOD2( GetCheckinManifestFrom, std::string( std::string, std::string ) );
    void MakeGetCheckinManifestFromReturn( std::string value );
    void ExpectGetCheckinManifestFromIsNotCalled();

};

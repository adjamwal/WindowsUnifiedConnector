#pragma once
#include "MocksCommon.h"
#include "IUtf8PathVerifier.h"

class MockUtf8PathVerifier : public IUtf8PathVerifier
{
public:
    MockUtf8PathVerifier();
    ~MockUtf8PathVerifier();

    MOCK_METHOD1( IsPathValid, bool( const std::filesystem::path& ) );
    void MakeIsPathValidReturn( bool value );
    void ExpectIsPathValidNotCalled();

    MOCK_METHOD1( PruneInvalidPathsFromList, void( std::vector<std::filesystem::path>& ) );
    void ExpectPruneInvalidPathsFromListIsNotCalled();
};
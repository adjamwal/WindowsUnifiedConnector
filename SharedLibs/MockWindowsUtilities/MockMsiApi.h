#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IMsiApi.h"

class MockMsiApi: public IMsiApi
{
public:
    MockMsiApi();
    ~MockMsiApi();

    MOCK_METHOD1( QueryProducts, int32_t( std::vector<MsiApiProductInfo>& products ) );
    void MakeFindProductsByNameReturn( int32_t value );

    MOCK_METHOD1( FindProductsByName, std::tuple<int32_t, std::vector<MsiApiProductInfo>>( std::wstring displayName ) );
    void MakeFindProductsByNameReturn( std::tuple<int32_t, std::vector<MsiApiProductInfo>> value );

    MOCK_METHOD1( FindProductsByPublisher, std::tuple<int32_t, std::vector<MsiApiProductInfo>>( std::wstring publisher ) );
    void MakeFindProductsByPublisherReturn( std::tuple<int32_t, std::vector<MsiApiProductInfo>> value );

    MOCK_METHOD2( FindProductsByNameAndPublisher, std::tuple<int32_t, std::vector<MsiApiProductInfo>>( 
        std::wstring displayName,
        std::wstring publisher ) );
    void MakeFindProductsByNameAndPublisherReturn( std::tuple<int32_t, std::vector<MsiApiProductInfo>> value );

    MOCK_METHOD1( FindRelatedProducts, std::tuple<int32_t, std::vector<MsiApiProductInfo>>( std::wstring upgradeCode ) );
    void MakeFindRelatedProductsReturn( std::tuple<int32_t, std::vector<MsiApiProductInfo>> value );

    MOCK_METHOD0( IsMsiServiceReadyforInstall, bool() );
    void MakeIsMsiServiceReadyforInstallReturn( bool value );
    void ExpectIsMsiServiceReadyforInstallIsNotCalled();
};
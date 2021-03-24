#include "pch.h"
#include "MockMsiApi.h"

MockMsiApi::MockMsiApi()
{
}

MockMsiApi::~MockMsiApi()
{
}

void MockMsiApi::MakeFindProductsByNameReturn( std::tuple<int32_t, std::vector<MsiApiProductInfo>> value )
{
    ON_CALL( *this, FindProductsByName( _ ) ).WillByDefault( Return( value ) );
}

void MockMsiApi::MakeFindProductsByPublisherReturn( std::tuple<int32_t, std::vector<MsiApiProductInfo>> value )
{
    ON_CALL( *this, FindProductsByPublisher( _ ) ).WillByDefault( Return( value ) );
}

void MockMsiApi::MakeFindProductsByNameAndPublisherReturn( std::tuple<int32_t, std::vector<MsiApiProductInfo>> value )
{
    ON_CALL( *this, FindProductsByNameAndPublisher( _, _ ) ).WillByDefault( Return( value ) );
}

void MockMsiApi::MakeFindRelatedProductsReturn( std::tuple<int32_t, std::vector<MsiApiProductInfo>> value )
{
    ON_CALL( *this, FindRelatedProducts( _ ) ).WillByDefault( Return( value ) );
}

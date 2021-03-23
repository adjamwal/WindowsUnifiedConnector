#include "pch.h"
#include <IMsiApi.h>
#include <MsiApi.h>
#include <WinApiWrapper.h>

class TestMsiApi : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_winApiWrapper.reset( new WinApiWrapper() );
        m_msiApi.reset( new MsiApi( *m_winApiWrapper ) );
    }

    void TearDown()
    {
        m_msiApi.reset();
        m_winApiWrapper.reset();
    }

    std::unique_ptr<WinApiWrapper> m_winApiWrapper;
    std::unique_ptr<MsiApi> m_msiApi;
};

TEST_F( TestMsiApi, TEST1 )
{
    auto[ret, list] = m_msiApi->FindProductsByPublisher(L"Cisco Systems, Inc.");

    ASSERT_EQ(list.size(), 1);

    auto [ret2, list2] = m_msiApi->FindRelatedProducts( L"{A00ADE60-6AA4-431F-8DB7-8F9386236798}" );

    ASSERT_EQ( list2.size(), 1 );
}
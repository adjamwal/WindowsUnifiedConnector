#include "gtest/gtest.h"
#include "CatalogListRetriever.h"
#include "MockPmCloud.h"
#include "MockUcidAdapter.h"
#include "MockCertsAdapter.h"
#include "MockPmConfig.h"

#include <memory>

class TestCatalogListRetriever : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_cloud.reset( new NiceMock<MockPmCloud>() );
        m_ucidAdapter.reset( new NiceMock<MockUcidAdapter>() );
        m_certs.reset( new NiceMock<MockCertsAdapter>() );
        m_config.reset( new NiceMock<MockPmConfig>() );
        m_patient.reset( new CatalogListRetriever( *m_cloud, *m_ucidAdapter, *m_certs, *m_config ) );
    }

    void TearDown()
    {
        m_patient.reset();
        m_cloud.reset();
        m_ucidAdapter.reset();
        m_certs.reset();
        m_config.reset();
    }

    std::unique_ptr<MockPmCloud> m_cloud;
    std::unique_ptr<MockUcidAdapter> m_ucidAdapter;
    std::unique_ptr<MockCertsAdapter> m_certs;
    std::unique_ptr<MockPmConfig> m_config;
    std::unique_ptr<CatalogListRetriever> m_patient;
};

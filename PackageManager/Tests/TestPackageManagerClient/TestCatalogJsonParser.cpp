#include "gtest/gtest.h"
#include "CatalogJsonParser.h"
#include "PmTypes.h"

#include <memory>

class TestCatalogJsonParser : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_patient.reset( new CatalogJsonParser() );
    }

    void TearDown()
    {
        m_patient.reset();
    }

    std::unique_ptr<CatalogJsonParser> m_patient;
};

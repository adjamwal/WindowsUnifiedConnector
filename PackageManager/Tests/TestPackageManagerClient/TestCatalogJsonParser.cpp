#include "gtest/gtest.h"
#include "CatalogJsonParser.h"
#include "PmTypes.h"
#include "StringUtil.h"

#include <memory>

class TestCatalogJsonParser : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_catalogDataset.clear();
        m_patient.reset( new CatalogJsonParser() );
    }

    void TearDown()
    {
        m_patient.reset();
        m_catalogDataset.clear();
    }

    std::unique_ptr<CatalogJsonParser> m_patient;
    std::vector<PmDiscoveryComponent> m_catalogDataset;
    const std::string m_testMissingPackagesCatalogJson = R"(
    {
        "products": []
    })";

    const std::string m_testCatalogJson = R"(
    {
        "packages": [
        {
            "name": "Immunet - Consumer connector",
                "product" : "Immunet",
                "version" : "7.4.0.20274",
                "configurables" : null
        },
        {
            "name": "Immunet - Consumer connector - Upgrade test",
            "product" : "Immunet",
            "version" : "99.0.98.0",
            "configurables" : null
        },
        {
            "name": "Immunet Unified Connector",
            "product" : "uc",
            "version" : "1.0.0.150",
            "configurables" : [
                {
                    "path": "<FOLDERID_ProgramFiles>\\Cisco\\Immunet\\UC\\Configuration\\uc.json",
                    "formats" : [
                        "json"
                    ] ,
                    "max_instances" : 0,
                    "min_instances" : 1
                },
                {
                    "path": "<FOLDERID_ProgramFiles>\\Cisco\\Immunet\\UC\\Configuration\\pm.json",
                    "formats" : [
                        "json"
                    ] ,
                    "max_instances" : 0,
                    "min_instances" : 1
                },
                {
                    "path": "<FOLDERID_ProgramFiles>\\Cisco\\Immunet\\UC\\Configuration\\id.json",
                    "formats" : [
                        "json"
                    ] ,
                    "max_instances" : 0,
                    "min_instances" : 1
                }
            ]
        },
        {
            "name": "Immunet Unified Connector",
            "product" : "uc",
            "version" : "98.0.0.0",
            "configurables" : [
                {
                    "path": "<FOLDERID_ProgramFiles>\\Cisco\\Immunet\\UC\\Configuration\\uc.json",
                    "formats" : [
                        "json"
                    ] ,
                    "max_instances" : 0,
                    "min_instances" : 1
                },
                {
                    "path": "<FOLDERID_ProgramFiles>\\Cisco\\Immunet\\UC\\Configuration\\pm.json",
                    "formats" : [
                        "json"
                    ] ,
                    "max_instances" : 0,
                    "min_instances" : 1
                },
                {
                    "path": "<FOLDERID_ProgramFiles>\\Cisco\\Immunet\\UC\\Configuration\\id.json",
                    "formats" : [
                        "json"
                    ] ,
                    "max_instances" : 0,
                    "min_instances" : 1
                }
            ]
        }
        ],
        "products": null
    }
    )";
};

TEST_F( TestCatalogJsonParser, WillParseCatalogJson )
{
    ASSERT_TRUE( m_patient->Parse( m_testCatalogJson, m_catalogDataset ) );
}

TEST_F( TestCatalogJsonParser, WillFailToParseEmptyCatalogJson )
{
    ASSERT_FALSE( m_patient->Parse( "", m_catalogDataset ) );
}

TEST_F( TestCatalogJsonParser, WillFailToParseCatalogJsonWithMissingPackages )
{
    ASSERT_FALSE( m_patient->Parse( m_testMissingPackagesCatalogJson, m_catalogDataset ) );
}

TEST_F( TestCatalogJsonParser, WillFailToParseMalformedCatalogJson )
{
    std::string malformed = m_testCatalogJson;
    StringUtil::ReplaceStringInPlace( malformed, ",", ";" );
    ASSERT_FALSE( m_patient->Parse( malformed, m_catalogDataset ) );
}

TEST_F( TestCatalogJsonParser, WillFailToParseConfirgurablesWithoutValidFormats )
{
    std::string malformed = m_testCatalogJson;
    StringUtil::ReplaceStringInPlace( malformed, "formats", "badvalue" );
    ASSERT_FALSE( m_patient->Parse( malformed, m_catalogDataset ) );
}

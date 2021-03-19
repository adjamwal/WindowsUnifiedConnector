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
        m_discoveryRules.clear();
        m_patient.reset( new CatalogJsonParser() );
    }

    void TearDown()
    {
        m_patient.reset();
        m_discoveryRules.clear();
    }

    std::unique_ptr<CatalogJsonParser> m_patient;
    std::vector<PmProductDiscoveryRules> m_discoveryRules;
    const std::string m_testPackagesCatalogJson = R"(
{
	"products": [
		{
			"product": "test-package-1",
			"configurables": [
				{
					"path": "*.json",
					"formats": [ "json" ],
					"max_instances": 10
				}
			],
			"discovery": [
				{
					"type": "msi",
					"name": "TestPackage",
					"vendor": "TestPackage"
				}
			]
		},
		{
			"product": "test-package-2",
			"discovery": [
				{
					"type": "upgrade_code",
					"code": "8c028f5dd91640b5a678f66b9789ec2b"
				}
			]
		},
		{
			"product": "test-package-3",
			"discovery": [
				{
					"type": "registry",
					"install": {
						"key": "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{AD3C0732-A954-43C1-A575-C439A6660AFC}\\InstallLocation"
					},
					"version": {
						"type": "keyType",
						"key": "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{AD3C0732-A954-43C1-A575-C439A6660AFC}\\DisplayVersion"
					}
				}
			]
		},
		{
			"product": "test-package-4",
			"discovery": [
				{
					"type": "msi",
					"name": "TestPackage",
					"vendor": "TestPackage"
				},
				{
					"type": "registry",
					"install": {
						"key": "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{AD3C0732-A954-43C1-A575-C439A6660AFC}\\InstallLocation"
					},
					"version": {
						"type": "keyType",
						"key": "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{AD3C0732-A954-43C1-A575-C439A6660AFC}\\DisplayVersion"
					}
				}
			]
		},
		{
			"product": "test-package-5",
			"discovery": [
				{
					"type": "msi",
					"name": "TestPackage",
					"vendor": "TestPackage"
				},
				{
					"type": "unknown"
				},
				{
					"type": "registry",
					"install": {
						"key": "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{AD3C0732-A954-43C1-A575-C439A6660AFC}\\InstallLocation"
					},
					"version": {
						"type": "keyType",
						"key": "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{AD3C0732-A954-43C1-A575-C439A6660AFC}\\DisplayVersion"
					}
				}
			]
		}
	]
})";

	const std::string m_InvalidConfigurable = R"(
{
	"products": [
		{
			"product": "test-package-1",
			"configurables": [
				{
					"path": "*.json",
					"formats": [ "json" ],
					"max_instances": 10
				}
			],
			"discovery": [
				{
					"type": "msi",
					"MissingName": "TestPackage",
					"vendor": "TestPackage"
				}
			]
		}
	]
})";

	const std::string m_InvalidMsiDiscovery = R"(
{
	"products": [
		{
			"product": "test-package-1",
			"configurables": [
				{
					"path": "*.json",
					"Missingformats": [ "json" ],
					"max_instances": 10
				}
			],
			"discovery": [
				{
					"type": "msi",
					"name": "TestPackage",
					"vendor": "TestPackage"
				}
			]
		}
	]
})";

	const std::string m_InvalidMsiUpgradeCodeDiscovery = R"(
{
	"products": [
		{
			"product": "test-package-2",
			"discovery": [
				{
					"type": "upgrade_code",
					"MissingCode": "8c028f5dd91640b5a678f66b9789ec2b"
				}
			]
		}
	]
})";

	const std::string m_InvalidRegistryCodeDiscovery = R"(
{
	"products": [
		{
			"product": "test-package-3",
			"discovery": [
				{
					"type": "registry",
					"install": {
						"key": "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{AD3C0732-A954-43C1-A575-C439A6660AFC}\\InstallLocation"
					},
					"version": {
						"type": "keyType",
						"Missingkey": "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{AD3C0732-A954-43C1-A575-C439A6660AFC}\\DisplayVersion"
					}
				}
			]
		}
	]
})";
};

TEST_F( TestCatalogJsonParser, WillParseCatalogJson )
{
    ASSERT_TRUE( m_patient->Parse( m_testPackagesCatalogJson, m_discoveryRules ) );
}

TEST_F( TestCatalogJsonParser, WillFailToParseEmptyCatalogJson )
{
    ASSERT_FALSE( m_patient->Parse( "", m_discoveryRules ) );
}

TEST_F( TestCatalogJsonParser, WillFailToParseMalformedCatalogJson )
{
    std::string malformed = m_testPackagesCatalogJson;
    StringUtil::ReplaceStringInPlace( malformed, ",", ";" );
    ASSERT_FALSE( m_patient->Parse( malformed, m_discoveryRules ) );
}

TEST_F( TestCatalogJsonParser, WillFailToParseConfirgurablesWithoutValidFormats )
{
	m_patient->Parse( m_InvalidConfigurable, m_discoveryRules );
	EXPECT_TRUE( m_discoveryRules.empty() );
}

TEST_F( TestCatalogJsonParser, WillParseDiscoveryMsiMethod )
{
	m_patient->Parse( m_testPackagesCatalogJson, m_discoveryRules );

	ASSERT_TRUE( m_discoveryRules.size() >= 1 );
	ASSERT_TRUE( m_discoveryRules[ 0 ].msi_discovery.size() == 1 );

	EXPECT_EQ( m_discoveryRules[ 0 ].msi_discovery[ 0 ].name, "TestPackage" );
	EXPECT_EQ( m_discoveryRules[ 0 ].msi_discovery[ 0 ].vendor, "TestPackage" );
}

TEST_F( TestCatalogJsonParser, WillParseDiscoveryMsiUpgradeCodeMethod )
{
	m_patient->Parse( m_testPackagesCatalogJson, m_discoveryRules );

	ASSERT_TRUE( m_discoveryRules.size() >= 2 );
	ASSERT_TRUE( m_discoveryRules[ 1 ].msiUpgradeCode_discovery.size() == 1 );

	EXPECT_EQ( m_discoveryRules[ 1 ].msiUpgradeCode_discovery[ 0 ].upgradeCode, "8c028f5dd91640b5a678f66b9789ec2b" );
}
TEST_F( TestCatalogJsonParser, WillParseDiscoveryRegistryMethod )
{
	m_patient->Parse( m_testPackagesCatalogJson, m_discoveryRules );

	ASSERT_TRUE( m_discoveryRules.size() >= 3);
	ASSERT_TRUE( m_discoveryRules[ 2 ].reg_discovery.size() == 1 );

	EXPECT_EQ( m_discoveryRules[ 2 ].reg_discovery[ 0 ].install.key, 
		"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{AD3C0732-A954-43C1-A575-C439A6660AFC}\\InstallLocation" );
	EXPECT_TRUE( m_discoveryRules[ 2 ].reg_discovery[ 0 ].install.type.empty() );

	EXPECT_EQ( m_discoveryRules[ 2 ].reg_discovery[ 0 ].version.key,
		"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{AD3C0732-A954-43C1-A575-C439A6660AFC}\\DisplayVersion" );
	EXPECT_EQ( m_discoveryRules[ 2 ].reg_discovery[ 0 ].version.type, "keyType" );
}

TEST_F( TestCatalogJsonParser, WillParseDiscoveryWithMultipleMethods )
{
	m_patient->Parse( m_testPackagesCatalogJson, m_discoveryRules );

	ASSERT_TRUE( m_discoveryRules.size() >= 4 );
	
	EXPECT_EQ( m_discoveryRules[ 3 ].msi_discovery.size(), 1 );
	EXPECT_EQ( m_discoveryRules[ 3 ].reg_discovery.size(), 1 );
}

TEST_F( TestCatalogJsonParser, ParseWillIgnoreUnknownMehods )
{
	m_patient->Parse( m_testPackagesCatalogJson, m_discoveryRules );

	ASSERT_TRUE( m_discoveryRules.size() >= 5 );

	EXPECT_EQ( m_discoveryRules[ 4 ].msi_discovery.size(), 1 );
	EXPECT_EQ( m_discoveryRules[ 4 ].reg_discovery.size(), 1 );
}

TEST_F( TestCatalogJsonParser, WillNotAddRuleWithInvalidMsiDiscovery )
{
	m_patient->Parse( m_InvalidMsiDiscovery, m_discoveryRules );
	EXPECT_TRUE( m_discoveryRules.empty() );
}

TEST_F( TestCatalogJsonParser, WillNotAddRuleWithInvalidMsiUpgradeCodeDiscovery )
{
	m_patient->Parse( m_InvalidMsiUpgradeCodeDiscovery, m_discoveryRules );
	EXPECT_TRUE( m_discoveryRules.empty() );
}

TEST_F( TestCatalogJsonParser, WillNotAddRuleWithInvalidRegistryDiscovery )
{
	m_patient->Parse( m_InvalidRegistryCodeDiscovery, m_discoveryRules );
	EXPECT_TRUE( m_discoveryRules.empty() );
}
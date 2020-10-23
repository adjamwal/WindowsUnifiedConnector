#include "gtest/gtest.h"
#include "PmManifest.h"
#include "MockFileUtil.h"
#include "PmTypes.h"

#include <memory>

static std::string _validManifest( R"(
{
  "packages": [
    {
      "installer_args": [
        "/arg1",
        "/arg2",
      ],
      "installer_signer_name": "Signer Name",
      "installer_type": "type1",
      "installer_uri": "Install URL",
      "package": "package name",
      "install_location": "Install Location",
      "installer_hash": "Install Hash",
    }
  ]
}
)" );

static std::string _minimalManifest( R"(
{
  "packages": [
    {
      "installer_type": "type1",
      "installer_uri": "Install URL",
      "package": "package name"
    }
  ]
}
)" );

static std::string _optionalManifest( R"(
{
  "packages": [
    {
      "installer_args": [
        "/arg1",
        "/arg2",
      ],
      "installer_signer_name": "Signer Name",
      "install_location": "Install Location",
      "installer_hash": "Install Hash",
    }
  ]
}
)" );

static std::string _multiPackageManifest( R"(
{
  "packages": [
    {
      "installer_type": "type1",
      "installer_uri": "Install URL1",
      "package": "package1"
    },
    {
      "installer_type": "type2",
      "installer_uri": "Install URL2",
      "package": "package2"
    }
  ]
}
)" );

class TestPmManifest: public ::testing::Test
{
protected:
    void SetUp()
    {
        m_patient.reset( new PmManifest() );
    }

    void TearDown()
    {
        m_patient.reset();
    }

    std::unique_ptr<PmManifest> m_patient;
};

TEST_F( TestPmManifest, WillParseManifest )
{
    EXPECT_EQ( m_patient->ParseManifest( _validManifest ), 0 );
}

TEST_F( TestPmManifest, WillGetPackages )
{
    m_patient->ParseManifest( _validManifest );

    auto packages = m_patient->GetPackageList();

    EXPECT_EQ( packages.size(), 1 );
}

TEST_F( TestPmManifest, WillGetPackageName )
{
    m_patient->ParseManifest( _validManifest );

    auto packages = m_patient->GetPackageList();

    EXPECT_EQ( packages[0].packageName, "package name" );
}

TEST_F( TestPmManifest, WillGetPackageUrl )
{
    m_patient->ParseManifest( _validManifest );

    auto packages = m_patient->GetPackageList();

    EXPECT_EQ( packages[ 0 ].installerUrl, "Install URL" );
}

TEST_F( TestPmManifest, WillGetPackageType )
{
    m_patient->ParseManifest( _validManifest );

    auto packages = m_patient->GetPackageList();

    EXPECT_EQ( packages[ 0 ].installerType, "type1" );
}

TEST_F( TestPmManifest, WillGetPackageArgs )
{
    m_patient->ParseManifest( _validManifest );

    auto packages = m_patient->GetPackageList();

    EXPECT_EQ( packages[ 0 ].installerArgs, "/arg1 /arg2 " );
}

TEST_F( TestPmManifest, WillGetPackageLocation )
{
    m_patient->ParseManifest( _validManifest );

    auto packages = m_patient->GetPackageList();

    EXPECT_EQ( packages[ 0 ].installLocation, "Install Location" );
}

TEST_F( TestPmManifest, WillGetPackageSigner )
{
    m_patient->ParseManifest( _validManifest );

    auto packages = m_patient->GetPackageList();

    EXPECT_EQ( packages[ 0 ].signerName, "Signer Name" );
}

TEST_F( TestPmManifest, WillGetPackageHash )
{
    m_patient->ParseManifest( _validManifest );

    auto packages = m_patient->GetPackageList();

    EXPECT_EQ( packages[ 0 ].installerHash, "Install Hash" );
}

TEST_F( TestPmManifest, WillAcceptMinimalPackage )
{
    EXPECT_EQ( m_patient->ParseManifest( _minimalManifest ), 0 );
}

TEST_F( TestPmManifest, WillNotAcceptPackageWithoutRequiredFields )
{
    EXPECT_NE( m_patient->ParseManifest( _optionalManifest ), 0 );
}

TEST_F( TestPmManifest, WillAcceptMultiplePackages )
{
    m_patient->ParseManifest( _multiPackageManifest );

    auto packages = m_patient->GetPackageList();

    EXPECT_EQ( packages.size(), 2 );
}
#include "gtest/gtest.h"
#include "CheckinFormatter.h"
#include "MockFileSysUtil.h"
#include "PmTypes.h"
#include "json/json.h"

#include <memory>

class TestCheckinFormatter : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_inventory.reset( new PackageInventory() );
        m_patient.reset( new CheckinFormatter() );
    }

    void TearDown()
    {
        m_patient.reset();
        m_inventory.reset();
    }

    void SetupInventory()
    {
        m_inventory->architecture = "architecture";
        m_inventory->platform = "platform";

        PmInstalledPackage package;
        package.product = "Name";
        package.version = "Version";

        PackageConfigInfo config;
        config.path = "path";
        config.sha256 = "sha";

        package.configs.push_back( config );
        m_inventory->packages.push_back( package );
    }

    void SetupMultiPackageInventory()
    {
        m_inventory->architecture = "architecture";
        m_inventory->platform = "platform";

        PackageConfigInfo config;
        PmInstalledPackage package;

        package.product = "Package1";
        package.version = "Version1";

        config.path = "path1";
        config.sha256 = "sha1";
        package.configs.push_back( config );

        config.path = "path2";
        config.sha256 = "sha2";
        package.configs.push_back( config );
        m_inventory->packages.push_back( package );

        package.configs.clear();
        package.product = "Package2";
        package.version = "Version2";


        config.path = "path3";
        config.sha256 = "sha3";
        package.configs.push_back( config );

        config.path = "path4";
        config.sha256 = "sha4";
        package.configs.push_back( config );
        m_inventory->packages.push_back( package );
    }

    std::unique_ptr<PackageInventory> m_inventory;
    std::unique_ptr<CheckinFormatter> m_patient;
};

TEST_F( TestCheckinFormatter, WillBuildJson )
{
    SetupInventory();

    std::string json = m_patient->GetJson( *m_inventory );

    EXPECT_NE( json.size(), 0 );
}

TEST_F( TestCheckinFormatter, WillBuildRootAttributes )
{
    SetupInventory();
    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root;

    std::string json = m_patient->GetJson( *m_inventory );

    jsonReader->parse( json.c_str(), json.c_str() + json.length(), &root, NULL );
    
    EXPECT_EQ( root[ "arch" ], m_inventory->architecture );
    EXPECT_EQ( root[ "platform" ], m_inventory->platform );
}


TEST_F( TestCheckinFormatter, WillBuildPackage )
{
    SetupInventory();
    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root;

    std::string json = m_patient->GetJson( *m_inventory );

    jsonReader->parse( json.c_str(), json.c_str() + json.length(), &root, NULL );

    EXPECT_EQ( root[ "installed" ][ 0 ][ "package" ], m_inventory->packages.front().product + "/"  + m_inventory->packages.front().version );
}

TEST_F( TestCheckinFormatter, WillBuildPackageConfig )
{
    SetupInventory();
    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root;

    std::string json = m_patient->GetJson( *m_inventory );

    jsonReader->parse( json.c_str(), json.c_str() + json.length(), &root, NULL );

    EXPECT_EQ( root[ "installed" ][ 0 ][ "configs" ][ 0 ][ "path" ], m_inventory->packages.front().configs.front().path.generic_u8string() );
    EXPECT_EQ( root[ "installed" ][ 0 ][ "configs" ][ 0 ][ "sha256" ], m_inventory->packages.front().configs.front().sha256 );
}

TEST_F( TestCheckinFormatter, WillBuildMultiPackageJson )
{
    SetupMultiPackageInventory();
    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root;

    std::string json = m_patient->GetJson( *m_inventory );

    jsonReader->parse( json.c_str(), json.c_str() + json.length(), &root, NULL );

    EXPECT_EQ( root[ "installed" ].size(), 2 );
    EXPECT_EQ( root[ "installed" ][ 0 ][ "configs" ].size(), 2 );
    EXPECT_EQ( root[ "installed" ][ 1 ][ "configs" ].size(), 2 );
}
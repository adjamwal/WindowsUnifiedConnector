#include "ComponentTestPacManBase.h"

class ComponentTestPacManConfigsDeployPath : public ComponentTestPacManBase
{
};

std::string _ucReponseConfigOnlyWithDeployPath( R"(
{
  "packages": [
    {
      "package": "uc/0.0.1",
      "install_location": "/install/location",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogInBhdGhfY29uZmlnLmpzb24iLAogICJkZXBsb3lfcGF0aCI6ICJjb25maWcuanNvbiIsCiAgInNoYTI1NiI6ICIyOTI3ZGIzNWIxODc1ZWYzYTQyNmQwNTI4MzYwOWIyZDk1ZDQyOWMwOTFlZTFhODJmMDY3MTQyM2E2NGQ4M2E0IiwKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "path_config.json",
          "deploy_path": "config.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",
          "verify_path": "verify.exe"
        }
      ]
    }
  ]
}
)" );

std::string _decodedConfigWithDeployPath( R"({
  "path": "path_config.json",
  "deploy_path": "config.json",
  "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",
  "verify_path": "verify.exe",
})" );

TEST_F( ComponentTestPacManConfigsDeployPath, PacManWillDecodeConfigWithDeployPath )
{
    bool pass = false;

    m_mockCloud->MakeGetReturn( true, _ucReponseConfigOnlyWithDeployPath, { 200, 0 } );
    m_mockCloud->MakeCheckinReturn( true, _ucReponseConfigOnlyWithDeployPath, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );

    ON_CALL( *m_mockSslUtil, DecodeBase64( _, _ ) ).WillByDefault( Invoke(
        []( const std::string& base64Str, std::vector<uint8_t>& output )
        {
            SslUtil sslUtil;
            return sslUtil.DecodeBase64( base64Str, output );
        }
    ) );

    EXPECT_CALL( *m_mockFileUtil, AppendFile( _, _, _ ) ).WillRepeatedly( Invoke(
        [this, &pass]( FileUtilHandle* handle, void* data, size_t dataLen )
        {
            std::string strData( ( char* )data, dataLen );
            EXPECT_EQ( _decodedConfigWithDeployPath, strData );

            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}

TEST_F( ComponentTestPacManConfigsDeployPath, PacManWillVerifyConfigWithDeployPath )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseConfigOnlyWithDeployPath, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockSslUtil->MakeCalculateSHA256Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" );

    EXPECT_CALL( *m_mockPlatformComponentManager, DeployConfiguration( _ ) ).WillRepeatedly( Invoke(
        [this, &pass]( const PackageConfigInfo& config )
        {
            EXPECT_EQ( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4", config.sha256 );
            EXPECT_EQ( "verify.exe", config.verifyBinPath );
            EXPECT_NE( std::string::npos, config.verifyPath.find( "tmpPmConf_" ) );

            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}

TEST_F( ComponentTestPacManConfigsDeployPath, PacManWillMoveConfigWithDeployPath )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseConfigOnlyWithDeployPath, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockPlatformComponentManager->MakeDeployConfigurationReturn( 0 );
    m_mockSslUtil->MakeCalculateSHA256Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" );

    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillRepeatedly( Invoke(
        [this, &pass]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
        {
            EXPECT_EQ( std::filesystem::path( "/install/location/config.json" ), newName );
            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}

std::string _ucReponseConfigDeployWithoutVerify( R"(
{
  "packages": [
    {
      "package": "uc/0.0.1",
      "install_location": "/install/location",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogInBhdGhfY29uZmlnLmpzb24iLAogICJkZXBsb3lfcGF0aCI6ICJjb25maWcuanNvbiIsCiAgInNoYTI1NiI6ICIyOTI3ZGIzNWIxODc1ZWYzYTQyNmQwNTI4MzYwOWIyZDk1ZDQyOWMwOTFlZTFhODJmMDY3MTQyM2E2NGQ4M2E0IiwKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "path_config.json",
          "deploy_path": "config.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",        
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacManConfigsDeployPath, PacManWillMoveConfigWithDeployPathWithoutVerification )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseConfigDeployWithoutVerify, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockSslUtil->MakeCalculateSHA256Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" );
    m_mockFileUtil->MakeFileExistsReturn( true );
    m_mockPlatformComponentManager->ExpectDeployConfigurationIsNotCalled();

    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillRepeatedly( Invoke(
        [this, &pass]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
        {
            EXPECT_EQ( std::filesystem::path( "/install/location/config.json" ), newName );
            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}

std::string _ucReponseWithConfigDeployPath( R"(
{
  "packages": [
    {
      "installer_args": [
        "/S",
        "/Q"
      ],
      "install_location": "/install/location",
      "installer_signer_name": "Cisco Systems, Inc.",
      "installer_sha256": "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
      "installer_type": "msi",
      "installer_uri": "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
      "package": "uc/0.0.1",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogInBhdGhfY29uZmlnLmpzb24iLAogICJkZXBsb3lfcGF0aCI6ICJjb25maWcuanNvbiIsCiAgInNoYTI1NiI6ICIyOTI3ZGIzNWIxODc1ZWYzYTQyNmQwNTI4MzYwOWIyZDk1ZDQyOWMwOTFlZTFhODJmMDY3MTQyM2E2NGQ4M2E0IiwKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "path_config.json",
          "deploy_path": "config.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",
          "verify_path": "verify.exe"
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacManConfigsDeployPath, PacManWillUpdatePackageAndConfigWithDeployPath )
{
    bool packageUpdated = false;
    bool configUpdated = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseWithConfigDeployPath, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockPlatformComponentManager->MakeDeployConfigurationReturn( 0 );

    ON_CALL( *m_mockFileUtil, FileExists( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( true ) );
    ON_CALL( *m_mockFileUtil, FileSize( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 100 ) );
    ON_CALL( *m_mockFileUtil, EraseFile( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 0 ) );

    ON_CALL( *m_mockSslUtil, CalculateSHA256( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" ) );
    ON_CALL( *m_mockSslUtil, CalculateSHA256( PathContains( "tmpPmConf_" ) ) ).WillByDefault( Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" ) );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillRepeatedly( Invoke(
        [this, &packageUpdated]( const PmComponent& package, std::string& error )
        {
            EXPECT_EQ( "/S /Q ", package.installerArgs );
            EXPECT_EQ( std::filesystem::path( "/install/location" ), package.installLocation );
            EXPECT_EQ( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3", package.installerHash );
            EXPECT_EQ( "Cisco Systems, Inc.", package.signerName );
            EXPECT_EQ( "msi", package.installerType );
            EXPECT_EQ( "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi", package.installerUrl );
            EXPECT_EQ( "uc/0.0.1", package.productAndVersion );

            packageUpdated = true;
            return 0;
        } ) );

    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillRepeatedly( Invoke(
        [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
        {
            configUpdated = true;
            m_cv.notify_one();

            return 0;
        } ) );

    EXPECT_CALL( *m_eventPublisher, Publish( _ ) ).Times( 2 );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( packageUpdated && configUpdated );

    PublishedEventHasExpectedData(
        "",
        pkgreconfig,
        "uc/0.0.1",
        0,
        "",
        0, //subErrCode
        "", //subErrType
        "",
        "",
        0,
        "config.json",
        "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",
        0
    );
}

std::string _ucReponseMultiPackageAndConfigWithDeployPath( R"(
{
  "packages": [
    {
      "installer_args": [
        "/S",
        "/Q"
      ],
      "install_location": "/install/location",
      "installer_signer_name": "Cisco Systems, Inc.",
      "installer_sha256": "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
      "installer_type": "msi",
      "installer_uri": "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
      "package": "uc/0.0.1",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogInBhdGhfY29uZmlnLmpzb24iLAogICJkZXBsb3lfcGF0aCI6ICJjb25maWcuanNvbiIsCiAgInNoYTI1NiI6ICIyOTI3ZGIzNWIxODc1ZWYzYTQyNmQwNTI4MzYwOWIyZDk1ZDQyOWMwOTFlZTFhODJmMDY3MTQyM2E2NGQ4M2E0IiwKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "path_p1_config1.json",
          "deploy_path": "p1_config1.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",
          "verify_path": "verify.exe"
        },
        {
          "contents": "ewogICJwYXRoIjogInBhdGhfY29uZmlnLmpzb24iLAogICJkZXBsb3lfcGF0aCI6ICJjb25maWcuanNvbiIsCiAgInNoYTI1NiI6ICIyOTI3ZGIzNWIxODc1ZWYzYTQyNmQwNTI4MzYwOWIyZDk1ZDQyOWMwOTFlZTFhODJmMDY3MTQyM2E2NGQ4M2E0IiwKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "path_p1_config2.json",
          "deploy_path": "p1_config2.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",
          "verify_path": "verify.exe"
        }
      ]
    },
    {
      "installer_args": [
        "/S",
        "/Q"
      ],
      "install_location": "/install/location",
      "installer_signer_name": "Cisco Systems, Inc.",
      "installer_sha256": "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
      "installer_type": "exe",
      "installer_uri": "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
      "package": "uc2/0.0.1",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogInBhdGhfY29uZmlnLmpzb24iLAogICJkZXBsb3lfcGF0aCI6ICJjb25maWcuanNvbiIsCiAgInNoYTI1NiI6ICIyOTI3ZGIzNWIxODc1ZWYzYTQyNmQwNTI4MzYwOWIyZDk1ZDQyOWMwOTFlZTFhODJmMDY3MTQyM2E2NGQ4M2E0IiwKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "path_p2_config1.json",
          "deploy_path": "p2_config1.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",
          "verify_path": "verify.exe"
        },
        {
          "contents": "ewogICJwYXRoIjogInBhdGhfY29uZmlnLmpzb24iLAogICJkZXBsb3lfcGF0aCI6ICJjb25maWcuanNvbiIsCiAgInNoYTI1NiI6ICIyOTI3ZGIzNWIxODc1ZWYzYTQyNmQwNTI4MzYwOWIyZDk1ZDQyOWMwOTFlZTFhODJmMDY3MTQyM2E2NGQ4M2E0IiwKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "path_p2_config2.json",
          "deploy_path": "p2_config2.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",
          "verify_path": "verify.exe"
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacManConfigsDeployPath, PacManWillUpdateMultiplePackageAndConfigWithDeployPath )
{
    int packageUpdated = 0;
    int configUpdated = 0;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseMultiPackageAndConfigWithDeployPath, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockPlatformComponentManager->MakeDeployConfigurationReturn( 0 );

    ON_CALL( *m_mockFileUtil, FileExists( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( true ) );
    ON_CALL( *m_mockFileUtil, FileSize( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 100 ) );
    ON_CALL( *m_mockFileUtil, EraseFile( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 0 ) );

    ON_CALL( *m_mockSslUtil, CalculateSHA256( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" ) );
    ON_CALL( *m_mockSslUtil, CalculateSHA256( PathContains( "tmpPmConf_" ) ) ).WillByDefault( Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" ) );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) )
        .WillOnce( Invoke(
            [this, &packageUpdated]( const PmComponent& package, std::string& error )
            {
                EXPECT_EQ( "/S /Q ", package.installerArgs );
                EXPECT_EQ( "/install/location", package.installLocation );
                EXPECT_EQ( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3", package.installerHash );
                EXPECT_EQ( "Cisco Systems, Inc.", package.signerName );
                EXPECT_EQ( "msi", package.installerType );
                EXPECT_EQ( "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi", package.installerUrl );
                EXPECT_EQ( "uc/0.0.1", package.productAndVersion );

                packageUpdated++;
                return 0;
            } ) )
        .WillOnce( Invoke(
            [this, &packageUpdated]( const PmComponent& package, std::string& error )
            {
                EXPECT_EQ( "/S /Q ", package.installerArgs );
                EXPECT_EQ( "/install/location", package.installLocation );
                EXPECT_EQ( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3", package.installerHash );
                EXPECT_EQ( "Cisco Systems, Inc.", package.signerName );
                EXPECT_EQ( "exe", package.installerType );
                EXPECT_EQ( "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi", package.installerUrl );
                EXPECT_EQ( "uc2/0.0.1", package.productAndVersion );

                packageUpdated++;
                return 0;
            } ) );

    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) )
        .WillOnce( Invoke(
            [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
            {
                EXPECT_EQ( std::filesystem::path( "/install/location/p1_config1.json" ), newName );
                configUpdated++;
                return 0;
            } ) )
        .WillOnce( Invoke(
            [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
            {
                EXPECT_EQ( std::filesystem::path( "/install/location/p1_config2.json" ), newName );
                configUpdated++;
                return 0;
            } ) )
        .WillOnce( Invoke(
            [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
            {
                EXPECT_EQ( std::filesystem::path( "/install/location/p2_config1.json" ), newName );
                configUpdated++;
                return 0;
            } ) )
        .WillOnce( Invoke(
            [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
            {
                EXPECT_EQ( std::filesystem::path( "/install/location/p2_config2.json" ), newName );
                configUpdated++;
                m_cv.notify_one();

                return 0;
            } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_EQ( 2, packageUpdated );
    EXPECT_EQ( 4, configUpdated );
}

std::string _ucReponseWithConfigDeployPathCloudData( R"(
{
  "packages": [
    {
      "installer_args": [
        "/S",
        "/Q"
      ],
      "installer_signer_name": "Cisco Systems, Inc.",
      "installer_sha256": "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
      "installer_type": "msi",
      "installer_uri": "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
      "package": "uc/0.0.1",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogInBhdGhfY29uZmlnLmpzb24iLAogICJkZXBsb3lfcGF0aCI6ICJjb25maWcuanNvbiIsCiAgInNoYTI1NiI6ICIyOTI3ZGIzNWIxODc1ZWYzYTQyNmQwNTI4MzYwOWIyZDk1ZDQyOWMwOTFlZTFhODJmMDY3MTQyM2E2NGQ4M2E0IiwKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "path_config.json",
          "deploy_path": "C:/Program Files/Cisco/SecureClient/UnifiedConnector/Configuration/cm_config.json"
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacManConfigsDeployPath, PacManWillUpdatePackageAndConfigCloudData )
{
    bool packageUpdated = false;
    bool configUpdated = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseWithConfigDeployPathCloudData, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockPlatformComponentManager->MakeDeployConfigurationReturn( 0 );

    ON_CALL( *m_mockFileUtil, FileExists( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( true ) );
    ON_CALL( *m_mockFileUtil, FileSize( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 100 ) );
    ON_CALL( *m_mockFileUtil, EraseFile( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 0 ) );

    ON_CALL( *m_mockSslUtil, CalculateSHA256( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" ) );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillRepeatedly( Invoke(
        [this, &packageUpdated]( const PmComponent& package, std::string& error )
        {
            EXPECT_EQ( "/S /Q ", package.installerArgs );
            EXPECT_EQ( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3", package.installerHash );
            EXPECT_EQ( "Cisco Systems, Inc.", package.signerName );
            EXPECT_EQ( "msi", package.installerType );
            EXPECT_EQ( "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi", package.installerUrl );
            EXPECT_EQ( "uc/0.0.1", package.productAndVersion );

            packageUpdated = true;
            return 0;
        } ) );
    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillRepeatedly( Invoke(
        [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
        {
            EXPECT_EQ( std::filesystem::path( "C:/Program Files/Cisco/SecureClient/UnifiedConnector/Configuration/cm_config.json" ), newName );
            configUpdated = true;
            m_cv.notify_one();

            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( packageUpdated && configUpdated );
}

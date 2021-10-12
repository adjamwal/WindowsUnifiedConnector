#include "ComponentTestPacManBase.h"

class ComponentTestPacManConfigsPath : public ComponentTestPacManBase
{
};

std::string _ucReponseNoConfig( R"(
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
      "package": "uc/0.0.1"
    }
  ] 
}
)" );

TEST_F( ComponentTestPacManConfigsPath, PacManWillUpdatePackage )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseNoConfig, { 200, 0 } );

    m_mockFileUtil->MakeFileExistsReturn( true );
    m_mockFileUtil->MakeFileSizeReturn( 100 );
    m_mockFileUtil->MakeEraseFileReturn( 0 );
    m_mockSslUtil->MakeCalculateSHA256Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillOnce( Invoke(
        [this, &pass]( const PmComponent& package, std::string& error )
        {
            EXPECT_EQ( "/S /Q ", package.installerArgs );
            EXPECT_EQ( "/install/location", package.installLocation );
            EXPECT_EQ( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3", package.installerHash );
            EXPECT_EQ( "Cisco Systems, Inc.", package.signerName );
            EXPECT_EQ( "msi", package.installerType );
            EXPECT_EQ( "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi", package.installerUrl );
            EXPECT_EQ( "uc/0.0.1", package.productAndVersion );

            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    //this fails (matcher gets correct params but passes them wrongly to the == operator)
    //EXPECT_CALL( *m_eventPublisher, Publish( CloudEventBuilderMatch( m_eventBuilder.get() ) ) ).Times( 1 );
    
    EXPECT_CALL( *m_eventPublisher, Publish( _ ) ).Times( 1 );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );

    PublishedEventHasExpectedData(
        "", //ucid
        pkginstall, //evtype
        "uc/0.0.1", //packageNameAndVersion
        0, //errCode
        "", //errMessage
        0, //subErrCode
        "", //subErrType
        "", //oldPath
        "", //oldHash
        0, //oldSize
        "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi", //newPath
        "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3", //newHash
        100 //newSize
    );
}

TEST_F( ComponentTestPacManConfigsPath, PacManWillRebootWhenPackageUpdateSetsRequiredFlag )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseNoConfig, { 200, 0 } );

    m_mockFileUtil->MakeFileExistsReturn( true );
    m_mockFileUtil->MakeFileSizeReturn( 100 );
    m_mockFileUtil->MakeEraseFileReturn( 0 );
    m_mockSslUtil->MakeCalculateSHA256Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillOnce( Invoke(
        [this, &pass]( const PmComponent& package, std::string& error )
        {
            return ERROR_SUCCESS_REBOOT_REQUIRED;
        } ) );

    EXPECT_CALL( *m_mockRebootHandler, HandleReboot( _ ) ).WillOnce( Invoke(
        [this, &pass]( bool rebootRequired )
        {
            pass = true;
            m_cv.notify_one();

            return true;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}

TEST_F( ComponentTestPacManConfigsPath, PacManWillSendRebootEventWhenRebootIsFlagged )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseNoConfig, { 200, 0 } );

    m_mockFileUtil->MakeFileExistsReturn( true );
    m_mockFileUtil->MakeFileSizeReturn( 100 );
    m_mockFileUtil->MakeEraseFileReturn( 0 );
    m_mockSslUtil->MakeCalculateSHA256Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillOnce( Invoke(
        [this, &pass]( const PmComponent& package, std::string& error )
        {
            return ERROR_SUCCESS_REBOOT_REQUIRED;
        } ) );

    EXPECT_CALL( *m_mockRebootHandler, HandleReboot( _ ) ).WillOnce( Invoke(
        [this, &pass]( bool rebootRequired )
        {
            pass = true;
            m_cv.notify_one();

            return true;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );

    PublishedEventHasExpectedData(
        "",
        pkginstall,
        "uc/0.0.1",
        UCPM_EVENT_SUCCESS_REBOOT_REQ,
        "Reboot required event",
        0, //subErrCode
        "", //subErrType
        "",
        "",
        0,
        "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
        "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
        100
    );
}

std::string _ucReponseConfigOnly( R"(
{
  "packages": [
    {
      "package": "uc/0.0.1",
      "install_location": "/install/location",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "config.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
          "verify_path": "verify.exe"
        }
      ]
    }
  ]
}
)" );

std::string _decodedConfig( R"({
  "path": "config.json",
  "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
  "verify_path": "verify.exe",
})" );

TEST_F( ComponentTestPacManConfigsPath, PacManWillDecodeConfig )
{
    bool pass = false;

    m_mockCloud->MakeCheckinReturn( true, _ucReponseConfigOnly, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );

    ON_CALL( *m_mockSslUtil, DecodeBase64( _, _ ) ).WillByDefault( Invoke(
        []( const std::string& base64Str, std::vector<uint8_t>& output )
        {
            SslUtil sslUtil;
            return sslUtil.DecodeBase64( base64Str, output );
        }
    ) );

    EXPECT_CALL( *m_mockFileUtil, AppendFile( _, _, _ ) ).WillOnce( Invoke(
        [this, &pass]( FileUtilHandle* handle, void* data, size_t dataLen )
        {
            std::string strData( ( char* )data, dataLen );
            EXPECT_EQ( _decodedConfig, strData );
                
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

TEST_F( ComponentTestPacManConfigsPath, PacManWillVerifyConfig )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseConfigOnly, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockSslUtil->MakeCalculateSHA256Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" );

    EXPECT_CALL( *m_mockPlatformComponentManager, DeployConfiguration( _ ) ).WillOnce( Invoke(
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

TEST_F( ComponentTestPacManConfigsPath, PacManWillMoveConfig )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseConfigOnly, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockPlatformComponentManager->MakeDeployConfigurationReturn( 0 );
    m_mockSslUtil->MakeCalculateSHA256Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" );

    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillOnce( Invoke(
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

std::string _ucReponseConfigWithoutVerify( R"(
{
  "packages": [
    {
      "package": "uc/0.0.1",
      "install_location": "/install/location",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "config.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",         
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacManConfigsPath, PacManWillMoveConfigWithoutVerification )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseConfigWithoutVerify, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockSslUtil->MakeCalculateSHA256Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" );
    m_mockFileUtil->MakeFileExistsReturn( true );
    m_mockPlatformComponentManager->ExpectDeployConfigurationIsNotCalled();
    
    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillOnce( Invoke(
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

std::string _ucReponseWithConfig( R"(
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
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "config.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
          "verify_path": "verify.exe"
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacManConfigsPath, PacManWillUpdatePackageAndConfig )
{
    bool packageUpdated = false;
    bool configUpdated = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseWithConfig, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockPlatformComponentManager->MakeDeployConfigurationReturn( 0 );

    ON_CALL( *m_mockFileUtil, FileExists( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( true ) );
    ON_CALL( *m_mockFileUtil, FileSize( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 100 ) );
    ON_CALL( *m_mockFileUtil, EraseFile( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 0 ) );

    ON_CALL( *m_mockSslUtil, CalculateSHA256( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" ) );
    ON_CALL( *m_mockSslUtil, CalculateSHA256( PathContains( "tmpPmConf_" ) ) ).WillByDefault( Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" ) );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillOnce( Invoke(
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

    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillOnce( Invoke(
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

std::string _ucReponseMultiPackageAndConfig( R"(
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
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "p1_config1.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
          "verify_path": "verify.exe"
        },
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "p1_config2.json",
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
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "p2_config1.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
          "verify_path": "verify.exe"
        },
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "p2_config2.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
          "verify_path": "verify.exe"
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacManConfigsPath, PacManWillUpdateMultiplePackageAndConfig )
{
    int packageUpdated = 0;
    int configUpdated = 0;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseMultiPackageAndConfig, { 200, 0 } );
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
                                EXPECT_EQ( std::filesystem::path(  "/install/location/p2_config2.json" ), newName );
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

std::string _ucReponseWithConfigCloudData( R"(
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
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "C:/Program Files/Cisco/SecureClient/UnifiedConnector/Configuration/cm_config.json"
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacManConfigsPath, PacManWillUpdatePackageAndConfigCloudData )
{
    bool packageUpdated = false;
    bool configUpdated = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseWithConfigCloudData, { 200, 0 } );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockPlatformComponentManager->MakeDeployConfigurationReturn( 0 );

    ON_CALL( *m_mockFileUtil, FileExists( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( true ) );
    ON_CALL( *m_mockFileUtil, FileSize( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 100 ) );
    ON_CALL( *m_mockFileUtil, EraseFile( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 0 ) );

    ON_CALL( *m_mockSslUtil, CalculateSHA256( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" ) );
    
    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillOnce( Invoke(
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
    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillOnce( Invoke(
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

std::string _ucReponseNoPackages( R"(
{
  "packages": null
}
)" );

TEST_F( ComponentTestPacManConfigsPath, PacManWillSendDicoveryList )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseNoPackages, { 200, 0 } );

    EXPECT_CALL( *m_mockPlatformComponentManager, GetInstalledPackages( _, _ ) )
        .WillOnce( Invoke(
        [this, &pass]( const std::vector<PmProductDiscoveryRules>& catalogRules, PackageInventory& packagesDiscovered )
        {
            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

//    std::this_thread::sleep_for( std::chrono::microseconds( 4000 ) );

    EXPECT_TRUE( pass );
}

TEST_F( ComponentTestPacManConfigsPath, PacManWillSendFailedEvents )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseNoPackages, { 200, 0 } );

    EXPECT_CALL( *m_eventPublisher, PublishFailedEvents() ).WillOnce( Invoke(
        [this, &pass]()
        {
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

TEST_F( ComponentTestPacManConfigsPath, PacManWillPruneInstallers )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseNoPackages, { 200, 0 } );
    EXPECT_CALL( *m_mockInstallerCacheMgr, PruneInstallers( _ ) ).WillOnce( Invoke(
        [this, &pass]( uint32_t ageInSeconds )
        {
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

TEST_F( ComponentTestPacManConfigsPath, PacManWillKickTheWatchdog )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseNoPackages, { 200, 0 } );

    EXPECT_CALL( *m_watchdog, Kick() ).Times(3).WillRepeatedly( Invoke(
        [this, &pass]( )
        {
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

TEST_F( ComponentTestPacManConfigsPath, PacManWillKickTheWatchdogOnNetworkError )
{
    bool pass = false;

    m_mockPlatformConfiguration->MakeGetIdentityTokenReturn( false );

    EXPECT_CALL( *m_watchdog, Kick() ).Times( 1 ).WillRepeatedly( Invoke(
        [this, &pass]()
        {
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

TEST_F( ComponentTestPacManConfigsPath, PacManWillUpdateCerts )
{
    bool pass = false;
    PmHttpExtendedResult eResult = { 0, 60, "curl" };

    SetupPacMacn();
    ON_CALL( *m_mockConfig, GetCloudCheckinIntervalMs() ).WillByDefault( Return( 1 ) );
    ON_CALL( *m_mockCloud, Get( _, _, _ ) ).WillByDefault( DoAll( SetArgReferee<2>( eResult ), Return( false ) ) );

    EXPECT_CALL( *m_certsAdapter, ReloadCerts() ).WillOnce( Invoke(
        [this, &pass]() {
            ON_CALL( *m_mockConfig, GetCloudCheckinIntervalMs() ).WillByDefault( Return( 1000 ) );
            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );
    
    m_patient->Start( "ConfigFile", "BootstrapFile" );
    
    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();
    
    EXPECT_TRUE( pass );
}

TEST_F( ComponentTestPacManConfigsPath, PacManWillNotUpdateCertsWhenRunningProperly )
{
    bool pass = false;
    m_mockCloud->MakeCheckinReturn( true, _ucReponseNoPackages, { 200, 0 } );
    m_certsAdapter->ExpectReloadCertsIsNotCalled();

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();
}
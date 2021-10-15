#include "PackageConfigProcessor.h"
#include "IFileSysUtil.h"
#include "ISslUtil.h"
#include "IUcidAdapter.h"
#include "CloudEventBuilder.h"
#include "CloudEventPublisher.h"
#include "PmTypes.h"
#include "PmLogger.h"
#include "PmConstants.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include <sstream>
#include "RandomUtil.h"
#include "PackageException.h"

PackageConfigProcessor::PackageConfigProcessor(
    IFileSysUtil& fileUtil,
    ISslUtil& sslUtil,
    IUcidAdapter& ucidAdapter,
    ICloudEventBuilder& eventBuilder,
    ICloudEventPublisher& eventPublisher )
    : m_fileUtil( fileUtil )
    , m_sslUtil( sslUtil )
    , m_ucidAdapter( ucidAdapter )
    , m_eventBuilder( eventBuilder )
    , m_eventPublisher( eventPublisher )
    , m_dependencies( nullptr )
    , m_fileCount( 0 )
{

}

PackageConfigProcessor::~PackageConfigProcessor()
{

}

void PackageConfigProcessor::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_dependencies = dep;
}

bool PackageConfigProcessor::ProcessConfig( PackageConfigInfo& config )
{
    m_eventPublisher.SetToken( m_ucidAdapter.GetAccessToken() );
    m_eventBuilder.Reset();
    m_eventBuilder.WithUCID( m_ucidAdapter.GetIdentity() );
    m_eventBuilder.WithPackageID( config.forProductAndVersion );

    if( config.deleteConfig ) {
        return RemoveConfig( config );
    }
    else {
        return AddConfig( config );
    }
}

bool PackageConfigProcessor::AddConfig( PackageConfigInfo& config )
{
    bool rtn = false;

    std::filesystem::path tempFilePath;
    std::vector<uint8_t> configData;
    std::filesystem::path verifiedTargetLocation;
    std::filesystem::path targetFolderIdPath;
    FileUtilHandle* handle = NULL;

    m_eventBuilder.WithType( CloudEventType::pkgreconfig );
    if( !config.deployPath.empty() && m_fileUtil.FileExists( config.installLocation / config.deployPath ) )
    {
        verifiedTargetLocation = config.installLocation / config.deployPath;
        targetFolderIdPath = config.unresolvedDeployPath;
        auto old_sha256 = m_sslUtil.CalculateSHA256( verifiedTargetLocation );
        m_eventBuilder.WithOldFile(
            config.unresolvedDeployPath,
            old_sha256.has_value() ? old_sha256.value() : "",
            m_fileUtil.FileSize( verifiedTargetLocation ) );
        m_eventBuilder.WithNewFile( config.unresolvedDeployPath, config.sha256, 0 );
    }
    else if( !config.cfgPath.empty() && m_fileUtil.FileExists( config.installLocation / config.cfgPath ) )
    {
        verifiedTargetLocation = config.installLocation / config.cfgPath;
        targetFolderIdPath = config.deployPath.empty() ? config.unresolvedCfgPath : config.unresolvedDeployPath;
        auto old_sha256 = m_sslUtil.CalculateSHA256( verifiedTargetLocation );
        m_eventBuilder.WithOldFile(
            config.unresolvedCfgPath,
            old_sha256.has_value() ? old_sha256.value() : "",
            m_fileUtil.FileSize( verifiedTargetLocation ) );
    }
    else if( !config.unresolvedDeployPath.empty() )
    {
        verifiedTargetLocation = config.installLocation / config.deployPath;
        targetFolderIdPath = config.unresolvedDeployPath;
    }
    else
    {
        verifiedTargetLocation = config.installLocation / config.cfgPath;
        targetFolderIdPath = config.unresolvedCfgPath;
    }

    m_eventBuilder.WithNewFile( targetFolderIdPath, config.sha256, 0 );

    try
    {
        if( m_sslUtil.DecodeBase64( config.contents, configData ) != 0 ) {

            throw PackageException( __FUNCTION__ ": Failed to decode " + config.contents, UCPM_EVENT_ERROR_CONFIG_DECODE );
        }

        tempFilePath = m_fileUtil.GetTempDir() / std::string( "tmpPmConf_" ).append( std::to_string( m_fileCount++ ) ).append( RandomUtil::GetString( 10 ) );
        config.verifyPath = tempFilePath.generic_u8string();

        if( ( handle = m_fileUtil.PmCreateFile( config.verifyPath ) ) == NULL )
        {
            throw PackageException( __FUNCTION__ ": Failed to create " + config.verifyPath, UCPM_EVENT_ERROR_CONFIG_CREATE );
        }

        size_t byteswritten = m_fileUtil.AppendFile( handle, configData.data(), configData.size() );
        m_fileUtil.CloseFile( handle );

        if( !byteswritten )
        {
            RemoveTempFile( config.verifyPath );
            throw PackageException( "Failed to write config data to " + config.verifyPath, UCPM_EVENT_ERROR_CONFIG_CREATE );
        }

        auto sha256 = m_sslUtil.CalculateSHA256( config.verifyPath );
        m_eventBuilder.WithNewFile(
            targetFolderIdPath,
            sha256.has_value() ? sha256.value() : config.sha256,
            m_fileUtil.FileSize( config.verifyPath )
        );

        // only validate hash if installerHash is not empty
        rtn = ( config.sha256.empty() || sha256 == config.sha256 ) &&
            ( config.verifyBinPath.empty() || ( m_dependencies->ComponentManager().DeployConfiguration( config ) == 0 ) ) &&
            //effectively move the temp file to the target destination
            ( m_fileUtil.Rename( config.verifyPath, verifiedTargetLocation ) == 0 ) &&
            ( m_dependencies->ComponentManager().ApplyBultinUsersReadPermissions( verifiedTargetLocation ) == 0 );

        if( !rtn )
        {
            RemoveTempFile( config.verifyPath );
            throw PackageException( __FUNCTION__ ": Failed to deploy configuration to " + verifiedTargetLocation.generic_u8string(),
                UCPM_EVENT_ERROR_CONFIG_DEPLOY );
        }
    }
    catch( PackageException& ex )
    {
        m_eventBuilder.WithError( ex.whatCode(), ex.what() );
        m_eventBuilder.WithSubError( ex.whatSubError().subErrorCode, ex.whatSubError().subErrorType );
        LOG_ERROR( "%s", ex.what() );
    }
    catch( std::exception& ex )
    {
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_UNDEFINED_EXCEPTION, ex.what() );
        LOG_ERROR( "%s", ex.what() );
    }
    catch( ... )
    {
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_UNDEFINED_EXCEPTION, "Unknown processing exception" );
        LOG_ERROR( "Unknown processing exception" );
    }

    m_eventPublisher.Publish( m_eventBuilder );

    return rtn;
}

bool PackageConfigProcessor::RemoveConfig( PackageConfigInfo& config )
{
    bool rtn = false;

    m_eventBuilder.WithType( CloudEventType::pkgreconfig );
    m_eventBuilder.WithOldFile(
        config.deployPath.empty() ? config.unresolvedCfgPath : config.unresolvedDeployPath,
        config.sha256,
        0 );

    try
    {
        if( !config.deployPath.empty() && m_fileUtil.FileExists( config.installLocation / config.deployPath ) )
        {
            EraseOrThrow(
                config.installLocation / config.deployPath,
                config.unresolvedDeployPath,
                config.sha256 );

            rtn = true;
        }
        else if( !config.cfgPath.empty() && m_fileUtil.FileExists( config.installLocation / config.cfgPath ) )
        {
            EraseOrThrow(
                config.installLocation / config.cfgPath,
                config.unresolvedCfgPath,
                config.sha256 );

            rtn = true;
        }
        else
        {
            throw PackageException( __FUNCTION__ ": Failed to resolve config for removal: " +
                config.deployPath.empty() ? config.cfgPath.generic_u8string() : config.deployPath.generic_u8string(),
                UCPM_EVENT_ERROR_CONFIG_RESOLVE );
        }
    }
    catch( PackageException& ex )
    {
        m_eventBuilder.WithError( ex.whatCode(), ex.what() );
        m_eventBuilder.WithSubError( ex.whatSubError().subErrorCode, ex.whatSubError().subErrorType );
        LOG_ERROR( "%s", ex.what() );
    }
    catch( std::exception& ex )
    {
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_UNDEFINED_EXCEPTION, ex.what() );
        LOG_ERROR( "%s", ex.what() );
    }
    catch( ... )
    {
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_UNDEFINED_EXCEPTION, "Unknown processing exception" );
        LOG_ERROR( "Unknown processing exception" );
    }

    m_eventPublisher.Publish( m_eventBuilder );

    return rtn;
}

void PackageConfigProcessor::EraseOrThrow(
    const std::filesystem::path& targetLocation,
    const std::filesystem::path& unresolvedPath,
    const std::string& configSha256 )
{
    auto sha256 = m_sslUtil.CalculateSHA256( targetLocation );
    m_eventBuilder.WithOldFile(
        unresolvedPath,
        sha256.has_value() ? sha256.value() : configSha256,
        m_fileUtil.FileSize( targetLocation ) );

    if( m_fileUtil.EraseFile( targetLocation ) != 0 )
    {
        throw PackageException( __FUNCTION__ ": Failed to remove config " +
            targetLocation.generic_u8string(), UCPM_EVENT_ERROR_CONFIG_REMOVE );
    }

    LOG_DEBUG( "Removed config file %s", targetLocation.generic_u8string().c_str() );
}

void PackageConfigProcessor::RemoveTempFile( const std::filesystem::path& tempFilePath )
{
    LOG_ERROR( "Removing %s", tempFilePath.generic_u8string().c_str() );
    if( m_fileUtil.EraseFile( tempFilePath ) != 0 ) {
        LOG_ERROR( "Failed to remove %s", tempFilePath.generic_u8string().c_str() );
    }
}

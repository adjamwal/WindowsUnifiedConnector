#include "PmManifest.h"
#include "PmTypes.h"
#include <json/json.h>
#include "PmLogger.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"

#define MANIFEST_FIELD_PACKAGE "package"
#define MANIFEST_FIELD_INSTALL_URI "installer_uri"
#define MANIFEST_FIELD_INSTALL_TYPE "installer_type"
#define MANIFEST_FIELD_INSTALL_ARGS "installer_args"
#define MANIFEST_FIELD_INSTALL_LOCATION "install_location"
#define MANIFEST_FIELD_INSTALL_SIGNER "installer_signer_name"
#define MANIFEST_FIELD_INSTALL_SHA "installer_sha256"
#define MANIFEST_FIELD_FILES "files"
#define MANIFEST_FIELD_CONFIG_PATH "path"
#define MANIFEST_FIELD_CONFIG_DEPLOY_PATH "deploy_path"
#define MANIFEST_FIELD_CONFIG_CONTENT "contents"
#define MANIFEST_FIELD_CONFIG_SHA "sha256"
#define MANIFEST_FIELD_CONFIG_VERIFY_PATH "verify_path"
#define MANIFEST_FIELD_CONFIG_DELETE "delete"

PmManifest::PmManifest() :
    m_dependencies( nullptr )
{

}

PmManifest::~PmManifest()
{

}

void PmManifest::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_dependencies = dep;
}

int32_t PmManifest::ParseManifest( const std::string& manifestJson )
{
    int32_t rtn = -1;
    std::lock_guard<std::mutex> lock( m_mutex );
    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root;
    std::string jsonError;

    if( !m_dependencies ) {
        LOG_ERROR( "Not initialized" );
        return rtn;
    }

    m_ComponentList.clear();

    if( manifestJson.empty() ) {
        LOG_DEBUG( "manifest contents is empty" );
        rtn = 0;
    }
    else if( !jsonReader->parse( manifestJson.c_str(), manifestJson.c_str() + manifestJson.length(), &root, &jsonError ) ) {
        LOG_ERROR( "Json Parse error %s", jsonError.c_str() );
    }
    else {
        try {
            if( root[ "packages" ].isArray() ) {
                for( Json::Value::ArrayIndex i = 0; i != root[ "packages" ].size(); i++ ) {
                    AddPackage( root[ "packages" ][ i ] );
                }

                rtn = 0;
            }
            else if( root[ "packages" ].isNull() ) {
                LOG_DEBUG( "manifest contents is empty" );
                rtn = 0;
            }
            else {
                LOG_ERROR( "packages array not found" );
            }
        }
        catch( std::exception& ex )
        {
            LOG_ERROR( "ParseManifest failed: %s", ex.what() );
        }
    }

    return rtn;
}

std::vector<PmComponent> PmManifest::GetPackageList()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    return m_ComponentList;
}

void PmManifest::AddPackage( Json::Value& packageJson )
{
    PmComponent package;

    // Required Data
    package.productAndVersion = GetJsonStringField( packageJson, MANIFEST_FIELD_PACKAGE, true );

    // Optional Data
    package.installerUrl = GetJsonStringField( packageJson, MANIFEST_FIELD_INSTALL_URI, false );
    package.installerType = GetJsonStringField( packageJson, MANIFEST_FIELD_INSTALL_TYPE, false );
    if( packageJson.isMember( MANIFEST_FIELD_INSTALL_ARGS ) ) {
        if( !packageJson[ MANIFEST_FIELD_INSTALL_ARGS ].isArray() ) {
            throw( std::invalid_argument( __FUNCTION__ ": Invalid Content: " MANIFEST_FIELD_INSTALL_ARGS ) );
        }

        for( Json::Value::ArrayIndex i = 0; i != packageJson[ MANIFEST_FIELD_INSTALL_ARGS ].size(); i++ ) {
            package.installerArgs += m_dependencies->ComponentManager().ResolvePath( packageJson[ MANIFEST_FIELD_INSTALL_ARGS ][ i ].asString() ) + " ";
        }
    }
    package.installLocation = std::filesystem::u8path( m_dependencies->ComponentManager().ResolvePath( GetJsonStringField( packageJson, MANIFEST_FIELD_INSTALL_LOCATION, false ) ) );
    package.signerName = GetJsonStringField( packageJson, MANIFEST_FIELD_INSTALL_SIGNER, false );
    package.installerHash = GetJsonStringField( packageJson, MANIFEST_FIELD_INSTALL_SHA, false );
    package.postInstallRebootRequired = false;

    if( packageJson[ MANIFEST_FIELD_FILES ].isArray() ) {
        for( Json::Value::ArrayIndex i = 0; i != packageJson[ MANIFEST_FIELD_FILES ].size(); i++ ) {
            AddConfigToPackage( packageJson[ MANIFEST_FIELD_FILES ][ i ], package );
        }
    }
    else {
        LOG_DEBUG( __FUNCTION__ ": config array not found" );
    }
    m_ComponentList.push_back( package );
}

void PmManifest::AddConfigToPackage( Json::Value& configJson, PmComponent& package )
{
    PackageConfigInfo config;
    config.deleteConfig = false;

    config.cfgPath = std::filesystem::u8path( 
        m_dependencies->ComponentManager().ResolvePath( 
            GetJsonStringField( configJson, MANIFEST_FIELD_CONFIG_PATH, true ) ) );
    config.unresolvedCfgPath = std::filesystem::u8path(
        GetJsonStringField( configJson, MANIFEST_FIELD_CONFIG_PATH, true ) );

    config.deployPath = std::filesystem::u8path(
        m_dependencies->ComponentManager().ResolvePath( 
            GetJsonStringField( configJson, MANIFEST_FIELD_CONFIG_DEPLOY_PATH, false ) ) );
    config.unresolvedDeployPath = std::filesystem::u8path(
        GetJsonStringField( configJson, MANIFEST_FIELD_CONFIG_DEPLOY_PATH, false ) );

    config.contents = GetJsonStringField( configJson, MANIFEST_FIELD_CONFIG_CONTENT, false );
    config.sha256 = GetJsonStringField( configJson, MANIFEST_FIELD_CONFIG_SHA, false );
    config.verifyBinPath = GetJsonStringField( configJson, MANIFEST_FIELD_CONFIG_VERIFY_PATH, false );

    config.installLocation = package.installLocation;
    config.signerName = package.signerName;

    if( configJson.isMember( MANIFEST_FIELD_CONFIG_DELETE ) && configJson[ MANIFEST_FIELD_CONFIG_DELETE ].isBool() ) {
        config.deleteConfig = configJson[ MANIFEST_FIELD_CONFIG_DELETE ].asBool();
    }

    if( !config.contents.empty() && config.deleteConfig ) {
        throw( std::invalid_argument( std::string( __FUNCTION__": Invalid config. Content provided and delete requested" ) ) );
    }

    package.configs.push_back( config );
}

std::string PmManifest::GetJsonStringField( Json::Value& packageJson, const char* field, bool required )
{
    if( packageJson.isMember( field ) ) {
        if( !packageJson[ field ].isString() ) {
            throw( std::invalid_argument( std::string( __FUNCTION__": Invalid Content: ") + field ) );
        }
    }
    else {
        if( required ) {
            throw( std::invalid_argument( std::string( __FUNCTION__": Missing Required Content: ") + field ) );
        }
        else {
            return "";
        }
    }

    LOG_DEBUG( "%s: %s", field, packageJson[ field ].asString().c_str() );
    return packageJson[ field ].asString();
}

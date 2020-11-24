#include "PmManifest.h"
#include "PmTypes.h"
#include <json/json.h>
#include "PmLogger.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"

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
    package.packageName = GetJsonStringField( packageJson, "package", true );

    // Optional Data
    package.installerUrl = GetJsonStringField( packageJson, "installer_uri", false );
    package.installerType = GetJsonStringField( packageJson, "installer_type", false );
    if( packageJson.isMember( "installer_args" ) ) {
        if( !packageJson[ "installer_args" ].isArray() ) {
            throw( std::invalid_argument( __FUNCTION__ ": Invalid Content: installer_args" ) );
        }

        for( Json::Value::ArrayIndex i = 0; i != packageJson[ "installer_args" ].size(); i++ ) {
            package.installerArgs += m_dependencies->ComponentManager().ResolvePath( packageJson[ "installer_args" ][ i ].asString() ) + " ";
        }
    }
    package.installLocation = m_dependencies->ComponentManager().ResolvePath( GetJsonStringField( packageJson, "install_location", false ) );
    package.signerName = GetJsonStringField( packageJson, "installer_signer_name", false );
    package.installerHash = GetJsonStringField( packageJson, "installer_hash", false );

    if( packageJson[ "files" ].isArray() ) {
        for( Json::Value::ArrayIndex i = 0; i != packageJson[ "files" ].size(); i++ ) {
            AddConfigToPackage( packageJson[ "files" ][ i ], package );
        }
    }
    else {
        LOG_DEBUG( "config array not found" );
    }
    m_ComponentList.push_back( package );
}

void PmManifest::AddConfigToPackage( Json::Value& configJson, PmComponent& package )
{
    PackageConfigInfo config;
    config.deleteConfig = false;

    config.path = m_dependencies->ComponentManager().ResolvePath( GetJsonStringField( configJson, "path", true ) );

    config.contents = GetJsonStringField( configJson, "contents", false );
    config.sha256 = GetJsonStringField( configJson, "sha256", false );
    config.verifyBinPath = GetJsonStringField( configJson, "verify_path", false );

    config.installLocation = package.installLocation;
    config.signerName = package.signerName;

    if( configJson.isMember( "delete" ) && configJson[ "delete" ].isBool() ) {
        config.deleteConfig = configJson[ "delete" ].asBool();
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

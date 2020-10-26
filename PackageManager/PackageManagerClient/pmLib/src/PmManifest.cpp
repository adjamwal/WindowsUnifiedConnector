#include "PmManifest.h"
#include "PmTypes.h"
#include <json/json.h>
#include "PmLogger.h"

PmManifest::PmManifest()
{

}

PmManifest::~PmManifest()
{

}

int32_t PmManifest::ParseManifest( const std::string& manifestJson )
{
    int32_t rtn = -1;
    std::lock_guard<std::mutex> lock( m_mutex );
    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root;
    std::string jsonError;

    m_ComponentList.clear();

    if( manifestJson.empty() ) {
        LOG_ERROR( "manifest contents is empty" );
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
            package.installerArgs += packageJson[ "installer_args" ][ i ].asString() + " ";
        }
    }
    package.installLocation = GetJsonStringField( packageJson, "install_location", false );
    package.signerName = GetJsonStringField( packageJson, "installer_signer_name", false );
    package.installerHash = GetJsonStringField( packageJson, "installer_hash", false );

    if( packageJson[ "configs" ].isArray() ) {
        for( Json::Value::ArrayIndex i = 0; i != packageJson[ "configs" ].size(); i++ ) {
            AddConfigToPackage( packageJson[ "configs" ][ i ], package );
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

    config.contents = GetJsonStringField( configJson, "contents", true );
    config.path = GetJsonStringField( configJson, "path", true );

    config.sha256 = GetJsonStringField( configJson, "sha256", false );
    config.verifyBinPath = GetJsonStringField( configJson, "verify_path", false );

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

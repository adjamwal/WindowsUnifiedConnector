#include "PmBootstrap.h"
#include "IFileSysUtil.h"
#include "PmLogger.h"
#include <json/json.h>

PmBootstrap::PmBootstrap( IFileSysUtil& fileUtil )
    : m_fileUtil( fileUtil )
{

}

PmBootstrap::~PmBootstrap()
{

}

int32_t PmBootstrap::LoadPmBootstrap( const std::string& filename )
{
    int rtn = -1;
    std::lock_guard<std::mutex> lock( m_mutex );

    std::string bsData = m_fileUtil.ReadFile( filename );

    rtn = ParseBootstrapConfig( bsData );

    if( rtn != 0 ) {
        LOG_ERROR( "Failed to parse %s", filename.c_str() );
    }

    return rtn;
}

std::string PmBootstrap::GetIdentifyUri()
{
    return m_idenitfyUri;
}

int32_t PmBootstrap::ParseBootstrapConfig( const std::string& bsConfig )
{
    int rtn = -1;

    std::unique_ptr<Json::CharReader> jsonReader( Json::CharReaderBuilder().newCharReader() );
    Json::Value root, pm;
    std::string jsonError;

    if( bsConfig.empty() ) {
        LOG_ERROR( "config contents is empty" );
    }
    else if( !jsonReader->parse( bsConfig.c_str(), bsConfig.c_str() + bsConfig.length(), &root, &jsonError ) ) {
        LOG_ERROR( "Json Parse error %s", jsonError.c_str() );
    }
    else {
        if( root.isMember( "identify_url" ) && root[ "identify_url" ].isString() ) {
            m_idenitfyUri = root[ "identify_url" ].asString();
            rtn = 0;
        }
        else {
            LOG_ERROR( "Json is malformed" );
        }
    }

    return rtn;
}
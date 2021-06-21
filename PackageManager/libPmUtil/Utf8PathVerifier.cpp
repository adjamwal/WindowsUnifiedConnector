#include "pch.h"
#include "Utf8PathVerifier.h"
#include "PmLogger.h"

Utf8PathVerifier::Utf8PathVerifier()
{ 
}

Utf8PathVerifier::~Utf8PathVerifier()
{

}

bool Utf8PathVerifier::IsPathValid( const std::filesystem::path& pathToVerify )
{
    bool rtn = false;

    try {
        //Path is validated by calling u8string(). It will throw if it's not valid
        LOG_DEBUG( "Validated Path %s", pathToVerify.u8string().c_str() );
        rtn = true;
    }
    catch( ... ) {
        //Need a better log message
        WLOG_ERROR( L"Invalid path detected %s", pathToVerify.wstring().c_str() );
    }

    return rtn;
}

void Utf8PathVerifier::PruneInvalidPathsFromList( std::vector<std::filesystem::path>& pathList )
{
    for( auto it = pathList.begin(); it != pathList.end(); ) {
        if( !IsPathValid( *it ) ) {
            it = pathList.erase( it );
        }
        else {
            it++;
        }
    }
}
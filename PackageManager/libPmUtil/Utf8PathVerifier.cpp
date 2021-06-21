#include "pch.h"
#include "Utf8PathVerifier.h"
#include "PmLogger.h"
#include <sstream>

Utf8PathVerifier::Utf8PathVerifier()
{ 
}

Utf8PathVerifier::~Utf8PathVerifier()
{

}

bool Utf8PathVerifier::IsPathValid( const std::filesystem::path& pathToVerify )
{
    bool rtn = false;
    std::string u8string;
    try {
        //Path is validated by calling u8string(). It will throw if it's not valid
        u8string = pathToVerify.u8string();
        rtn = true;
    }
    catch( ... ) {
        std::wstringstream pathHex;
        for( int i = 0; i < pathToVerify.wstring().length(); i++ ) {
            pathHex << L"0x" << std::setfill( L'0' ) << std::setw( 4 ) << std::hex << ( int )pathToVerify.wstring()[ i ] << L" ";
        }

        WLOG_ERROR( L"Invalid path detected(hex) %s", pathHex.str().c_str() );
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

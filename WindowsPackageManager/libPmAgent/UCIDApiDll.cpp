#include "pch.h"

#include "UCIDApiDll.h"
#include "ICodeSignVerifier.h"
#include "WindowsUtilities.h"
#include "StringUtil.h"
#include <IUcLogger.h>
#include "PmTypes.h"
#include "CmConstants.h"

UCIDApiDll::UCIDApiDll( ICodesignVerifier& codeSignVerifier )
    : m_codeSignVerifier( codeSignVerifier )
    , m_api( 0 )
    , m_getIdFunc( NULL )
    , m_getTokenFunc( NULL )
    , m_refreshTokenFunc( NULL )
    , m_getUrlFunc( NULL )
    , m_loadedDllName()
    , m_isModuleLoaded( false )
{

}

UCIDApiDll::~UCIDApiDll()
{
    UnloadApi();
    UnloadDll();
}

int32_t UCIDApiDll::GetId( std::string& id )
{
    LoadApi();

    int bufsz = 0;

    cmid_result_t res = m_getIdFunc( NULL, &bufsz );

    if( res == CMID_RES_INSUFFICIENT_LEN ) {
        char* myucid = ( char* )malloc( bufsz );

        res = m_getIdFunc( myucid, &bufsz );

        if( myucid )
        {
            id.assign( myucid, bufsz - 1 );
            free( myucid );
        }
    }
    if( res != CMID_RES_SUCCESS )
    {
        return res;
    }

    return res;
}

int32_t UCIDApiDll::GetToken( std::string& token )
{
    LoadApi();

    int bufsz = 0;

    cmid_result_t res = m_getTokenFunc( NULL, &bufsz );

    if( res == CMID_RES_INSUFFICIENT_LEN ) {
        char* myucid = ( char* )malloc( bufsz );

        res = m_getTokenFunc( myucid, &bufsz );

        if( myucid )
        {
            token.assign( myucid, bufsz );
            free( myucid );
        }
    }
    if( res != CMID_RES_SUCCESS )
    {
        return res;
    }

    return res;
}

int32_t UCIDApiDll::RefreshToken()
{
    LoadApi();

    return m_refreshTokenFunc();
}

cmid_result_t UCIDApiDll::GetUrl( cmid_url_type_t urlType, std::string& url )
{
    cmid_result_t result = CMID_RES_GENERAL_ERROR;
    int urlSize = 1024;
    char* tmpUrl = ( char* )malloc( urlSize );

    if( !tmpUrl ) {
        LOG_CRITICAL( "Failed to allocate %d bytes", urlSize );
    }
    else {
        result = m_getUrlFunc( urlType, tmpUrl, &urlSize );
        if( result == CMID_RES_INSUFFICIENT_LEN ) {
            tmpUrl = ( char* )realloc( tmpUrl, urlSize );
            if( !tmpUrl ) {
                LOG_CRITICAL( "Failed to allocate %d bytes", urlSize );
            }
            else {
                result = m_getUrlFunc( urlType, tmpUrl, &urlSize );
                if( result == CMID_RES_SUCCESS ) {
                    url = tmpUrl;
                }
            }
        }
        else if ( result == CMID_RES_SUCCESS ) {
            url = tmpUrl;
        }
    }

    if( tmpUrl ) {
        free( tmpUrl );
        tmpUrl = NULL;
    }

    return result;
}

int32_t UCIDApiDll::GetUrls( PmUrlList& urls )
{
    int32_t rtn = CMID_RES_SUCCESS;
    int32_t tmpRtn = 0;
    LoadApi();

    tmpRtn = GetUrl( CMID_EVENT_URL, urls.eventUrl );
    if( tmpRtn != CMID_RES_SUCCESS ) {
        LOG_ERROR( "Failed to fetch event url %d", tmpRtn );
        rtn = CMID_RES_GENERAL_ERROR;
    }

    tmpRtn = GetUrl( CMID_CHECKIN_URL, urls.checkinUrl );
    if( tmpRtn != CMID_RES_SUCCESS ) {
        LOG_ERROR( "Failed to fetch checking url %d", tmpRtn );
        rtn = CMID_RES_GENERAL_ERROR;
    }

    tmpRtn = GetUrl( CMID_CATALOG_URL, urls.catalogUrl );
    if( tmpRtn != CMID_RES_SUCCESS ) {
        LOG_ERROR( "Failed to fetch catalog url %d", tmpRtn );
        rtn = CMID_RES_GENERAL_ERROR;
    }

    LOG_DEBUG( "Event Url %s Checkin Url %s Catalog Url %s", urls.eventUrl.c_str(), urls.checkinUrl.c_str(), urls.catalogUrl.c_str() );

    return rtn;
}

bool UCIDApiDll::LoadDll( const std::wstring dllPath )
{
    std::wstring dllDir;

    std::wstring::size_type pos = dllPath.find_last_of( L"/\\" );
    if( pos == std::wstring::npos || pos >= dllPath.size() )
    {
        WLOG_ERROR( L"Invalid dll path specified: %s", dllPath.c_str() );
        return false;
    }

    dllDir = dllPath.substr( 0, pos );
    m_loadedDllName = dllPath.substr( pos + 1, dllPath.size() - pos - 1 );

    if( m_api != 0 )
    {
        WLOG_ERROR( L"Dll already loaded: %s", dllPath.c_str() );
        return false;
    }

    if( m_codeSignVerifier.Verify( dllPath, SIGNER_CISCO, SIGTYPE_DEFAULT ) != CodesignStatus::CODE_SIGNER_SUCCESS ) {
        WLOG_ERROR( L"Failed to verify dll signature: %s", dllPath.c_str() );
        return false;
    }

    WLOG_DEBUG( L"Loading dll %s from %s", m_loadedDllName.c_str(), dllDir.c_str() );

    SetDllDirectory( dllDir.c_str() );

    m_api = LoadLibrary( m_loadedDllName.c_str() );
    if( !m_api ) {
        WLOG_ERROR( L"LoadLibrary() call failed. Error %d", GetLastError() );
        SetDllDirectory( NULL );
        return false;
    }

    SetDllDirectory( NULL );

    m_getIdFunc = ( GetIdFunc )GetProcAddress( m_api, CMID_API_GET_ID_FUNCTION_NAME );
    if( m_getIdFunc == NULL )
    {
        throw std::exception( "Couldn't bind to Get Id dll function. Error %d", GetLastError() );
    }

    m_getTokenFunc = ( GetTokenFunc )GetProcAddress( m_api, CMID_API_GET_TOKEN_FUNCTION_NAME );
    if( m_getTokenFunc == NULL )
    {
        throw std::exception( "Couldn't bind to Get Token dll function. Error %d", GetLastError() );
    }

    m_refreshTokenFunc = ( RefreshTokenFunc )GetProcAddress( m_api, CMID_API_REFRESH_TOKEN_FUNCTION_NAME );
    if( m_refreshTokenFunc == NULL )
    {
        throw std::exception( "Couldn't bind to Refresh Token dll function. Error %d", GetLastError() );
    }

    m_getUrlFunc = ( GetUrlFunc )GetProcAddress( m_api, CMID_API_GET_URL_FUNCTION_NAME );
    if( m_getUrlFunc == NULL ) {
        throw std::exception( "Couldn't bind to Get Url dll function. Error %d", GetLastError() );
    }

    return true;
}

void UCIDApiDll::UnloadDll()
{
    if( !m_api )
    {
        return;
    }

    WLOG_DEBUG( L"UnLoading %s", m_loadedDllName.c_str() );

    if( FreeLibrary( m_api ) == 0 )
    {
        WLOG_ERROR( L"FreeLibrary() call failed. Error %d", GetLastError() );
    }

    m_api = 0;
    m_getIdFunc = NULL;
    m_getTokenFunc = NULL;
    m_refreshTokenFunc = NULL;
    m_loadedDllName.clear();
}


bool UCIDApiDll::LoadApi()
{
    bool ret = false;

    if( m_isModuleLoaded )
    {
        WLOG_DEBUG( L"Cloud Management ID Api already loaded." );
        return true;
    }

    std::wstring dllFullPath;

    if( !WindowsUtilities::ReadRegistryStringW( HKEY_LOCAL_MACHINE, CMID_API_DLL_KEY, L"ucidapi", dllFullPath ) )
    {
        WLOG_ERROR( L"Failed to read Cloud Management ID Api data from registry" );
        return false;
    }

    try
    {
        if( !LoadDll( dllFullPath ) )
        {
            WLOG_ERROR( L"Failed to load %s", dllFullPath.c_str() );
            return false;
        }

        ret = true;
    }
    catch( std::exception& ex )
    {
        LOG_ERROR( "Exception: %s", ex.what() );
    }

    m_isModuleLoaded = true;

    WLOG_DEBUG( L"Cloud Management ID Api loaded." );

    return ret;
}

void UCIDApiDll::UnloadApi()
{
    UnloadDll();

    if( !m_isModuleLoaded )
    {
        WLOG_DEBUG( L"Cloud Management ID Api already unloaded." );
        return;
    }

    m_isModuleLoaded = false;

    WLOG_DEBUG( L"Cloud Management ID Api unloaded." );
}

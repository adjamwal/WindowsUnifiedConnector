#include "pch.h"

#include "UCIDApiDll.h"
#include "ICodeSignVerifier.h"
#include "WindowsUtilities.h"
#include <IUcLogger.h>

#define UCID_API_GET_ID_FUNCTION_NAME "ucid_get_id"
#define UCID_API_GET_TOKEN_FUNCTION_NAME "ucid_get_token"
#define UCID_API_REFRESH_TOKEN_FUNCTION_NAME "ucid_refresh_token"

UCIDApiDll::UCIDApiDll( ICodesignVerifier& codeSignVerifier )
    : m_codeSignVerifier( codeSignVerifier )
    , m_api( 0 )
    , m_getIdFunc( NULL )
    , m_getTokenFunc(NULL)
    , m_refreshTokenFunc(NULL)
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

    ucid_result_t res = m_getIdFunc(NULL, &bufsz);

    if (res == UCID_RES_INSUFFICIENT_LEN) {
        char* myucid = (char*)malloc(bufsz);

        res = m_getIdFunc(myucid, &bufsz);

        if (myucid)
        {
            id.assign(myucid, bufsz);
            free(myucid);
        }
    }
    if (res != UCID_RES_SUCCESS) 
    {
        return res;
    }

    return res;
}

int32_t UCIDApiDll::GetToken(std::string& token )
{
    LoadApi();

    int bufsz = 0;

    ucid_result_t res = m_getIdFunc(NULL, &bufsz);

    if (res == UCID_RES_INSUFFICIENT_LEN) {
        char* myucid = (char*)malloc(bufsz);

        res = m_getIdFunc(myucid, &bufsz);

        if (myucid)
        {
            token.assign(myucid, bufsz);
            free(myucid);
        }
    }
    if (res != UCID_RES_SUCCESS)
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
    if( !m_api) {
        WLOG_ERROR( L"LoadLibrary() call failed. Error %d", GetLastError() );
        SetDllDirectory( NULL );
        return false;
    }

    SetDllDirectory( NULL );

    m_getIdFunc = ( GetIdFunc )GetProcAddress( m_api, UCID_API_GET_ID_FUNCTION_NAME);
    if(m_getIdFunc == NULL )
    {
        throw std::exception( "Couldn't bind to Get Id dll function. Error %d", GetLastError() );
    }

    m_getTokenFunc = ( GetTokenFunc )GetProcAddress( m_api, UCID_API_GET_TOKEN_FUNCTION_NAME );
    if(m_getTokenFunc == NULL )
    {
        throw std::exception( "Couldn't bind to Get Token dll function. Error %d", GetLastError() );
    }

    m_refreshTokenFunc = ( RefreshTokenFunc )GetProcAddress( m_api, UCID_API_REFRESH_TOKEN_FUNCTION_NAME );
    if (m_refreshTokenFunc == NULL)
    {
        throw std::exception("Couldn't bind to Refresh Token dll function. Error %d", GetLastError());
    }

    return true;
}

void UCIDApiDll::UnloadDll()
{
    if( !m_api)
    {
        return;
    }

    WLOG_DEBUG( L"UnLoading %s", m_loadedDllName.c_str() );

    if( FreeLibrary(m_api) == 0 )
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
        WLOG_DEBUG( L"UnifiedConnectorID Api already loaded." );
        return true;
    }

    std::wstring ucidDllDir;
    std::wstring dllFullPath;
    if( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, L"Software\\Cisco\\SecureXYZ\\UnifiedConnector\\UCID", L"Path", ucidDllDir ) )
    {
        WLOG_ERROR( L"Failed to read UnifiedConnectorID Api data from registry" );
        return false;
    }

    dllFullPath = ucidDllDir;
    if(WindowsUtilities::Is64BitWindows() )
    {
        dllFullPath.append( L"x64\\" );
    }
    else
    {
        dllFullPath.append( L"x86\\" );
    }
    dllFullPath.append( L"ucidapi.dll" );

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

    WLOG_DEBUG( L"UnifiedConnectorID Api loaded." );

    return ret;
}

void UCIDApiDll::UnloadApi()
{
    UnloadDll();

    if( !m_isModuleLoaded )
    {
        WLOG_ERROR( L"UnifiedConnectorID Api already unloaded." );
        return;
    }

    m_isModuleLoaded = false;

    WLOG_DEBUG( L"UnifiedConnectorID Api unloaded." );
}

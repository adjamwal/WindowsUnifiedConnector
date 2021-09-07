#include "pch.h"

#include "UCMCPLoader.h"
#include "IUCLogger.h"
#include "ICodeSignVerifier.h"
#include "WindowsUtilities.h"

UCMCPLoader::UCMCPLoader( ICodesignVerifier& codeSignVerifier )
    : m_codeSignVerifier( codeSignVerifier )
    , m_controlLib( 0 )
    , m_createModule( NULL )
    , m_releaseModule( NULL )
    , m_loadedDllName()
    , m_context( { 0 } )
    , m_isModuleLoaded( false )
{
}

UCMCPLoader::~UCMCPLoader()
{
    UnloadControlModule();
    UnloadDll();
}

bool UCMCPLoader::LoadDll( const std::wstring dllPath )
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

    if( m_controlLib != 0 )
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

    m_controlLib = LoadLibrary( m_loadedDllName.c_str() );
    if( !m_controlLib ) {
        WLOG_ERROR( L"LoadLibrary() call failed. Error %d", GetLastError() );
        SetDllDirectory( NULL );
        return false;
    }

    SetDllDirectory( NULL );

    m_createModule = (CreateModuleFunc )GetProcAddress( m_controlLib, "CreateModuleInstance" );
    if( m_createModule == NULL )
    {
        throw std::exception( "Couldn't bind to CreateModuleInstance dll function. Error %d", GetLastError() );
    }

    m_releaseModule = (ReleaseModuleFunc )GetProcAddress( m_controlLib, "ReleaseModuleInstance" );
    if( m_releaseModule == NULL )
    {
        throw std::exception( "Couldn't bind to ReleaseModuleInstance dll function. Error %d", GetLastError() );
    }

    return true;
}

void UCMCPLoader::UnloadDll()
{
    if( !m_controlLib )
    {
        return;
    }

    WLOG_DEBUG( L"UnLoading %s", m_loadedDllName.c_str() );

    if( FreeLibrary( m_controlLib ) == 0 )
    {
        WLOG_ERROR( L"FreeLibrary() call failed. Error %d", GetLastError() );
    }

    m_controlLib = 0;
    m_createModule = NULL;
    m_releaseModule = NULL;
    m_loadedDllName.clear();
}

PM_MODULE_RESULT_T UCMCPLoader::CreateModule( PM_MODULE_CTX_T* pPM_MODULE_CTX )
{
    return m_createModule( pPM_MODULE_CTX );
}

PM_MODULE_RESULT_T UCMCPLoader::ReleaseModule( PM_MODULE_CTX_T* pPM_MODULE_CTX )
{
    return m_releaseModule( pPM_MODULE_CTX );
}

void UCMCPLoader::LoadControlModule()
{
    if( m_isModuleLoaded )
    {
        WLOG_ERROR( L"PackageManager Control Module already running." );
        return;
    }

    std::wstring dllFullPath;
    if( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, L"Software\\Cisco\\SecureClient\\Cloud Management\\CMPM", L"DllPath", dllFullPath ) )
    {
        WLOG_ERROR( L"Failed to read PackageManager Control Module data from registry" );
        return;
    }

    std::wstring pmPath(WindowsUtilities::GetDirPath( dllFullPath ) );

    std::wstring pmConfigPath;

    if ( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, L"Software\\Cisco\\SecureClient\\Cloud Management\\config", L"path", pmConfigPath ) )
    {
        WLOG_ERROR( L"Failed to read config path from registry" );
        return;
    }

    try
    {
        if( !LoadDll( dllFullPath ) )
        {
            WLOG_ERROR( L"Failed to load %s", dllFullPath.c_str() );
            return;
        }
    }
    catch( std::exception& ex )
    {
        LOG_ERROR( "Exception: %s", ex.what() );
    }

    m_context.nVersion = PM_MODULE_INTERFACE_VERSION;
    if( m_context.fpInit )
    {
        m_context.fpInit();
    }

    PM_MODULE_RESULT_T result;

    if( ( result = CreateModule( &m_context ) ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to load PackageManager Control Module: CreateModuleInstance() returned %d.", result );
        return;
    }

    if( ( result = m_context.fpSetOption( PM_MODULE_OPTION_LOG_LEVEL, GetUcLogger(), sizeof( GetUcLogger() ) ) ) != PM_MODULE_SUCCESS ) {
        WLOG_ERROR( L"Failed to set option PM_MODULE_OPTION_LOG_LEVEL", result );
    }

    if( ( result = m_context.fpStart( pmPath.c_str(), pmPath.c_str(), pmConfigPath.c_str() ) ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to start PackageManager Control Module: fpStart() returned %d.", result );
        return;
    }

    m_isModuleLoaded = true;

    WLOG_DEBUG( L"PackageManager Control Module loaded and started." );
}

void UCMCPLoader::UnloadControlModule()
{
    if( !m_isModuleLoaded )
    {
        WLOG_ERROR( L"PackageManager Control Module already released." );
        return;
    }

    PM_MODULE_RESULT_T result;

    if( ( result = m_context.fpStop() ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to stop PackageManager Control Module: fpStop() returned %d.", result );
        return;
    }

    if( ( result = ReleaseModule( &m_context ) ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to release PackageManager Control Module: ReleaseModuleInstance() returned %d.", result );
        return;
    }

    m_isModuleLoaded = false;

    WLOG_DEBUG( L"PackageManager Control Module stopped and released." );
}

#include "stdafx.h"

#include "UCIDLoader.h"
#include "IUCLogger.h"
#include "ICodeSignVerifier.h"
#include "HelperFunctions.h"

#define UCID_MODULE_INTERFACE_VERSION 1u
#define UCID_MODULE_CONFIG_FILENAME L"config.xml"

UCIDLoader::UCIDLoader( ICodesignVerifier& codeSignVerifier, IUcLogger& logger )
    : m_codeSignVerifier( codeSignVerifier )
    , m_logger( logger )
    , m_controlLib( 0 )
    , m_createModule( NULL )
    , m_releaseModule( NULL )
    , m_loadedDllName()
    , m_context( { 0 } )
    , m_isModuleLoaded( false )
{
}

UCIDLoader::~UCIDLoader()
{
    UnloadControlModule();
    UnloadDll();
}

bool UCIDLoader::LoadDll( const std::wstring dllPath )
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

void UCIDLoader::UnloadDll()
{
    if( !m_controlLib )
    {
        return;
    }

    LOG_DEBUG( "UnLoading %s", m_loadedDllName.c_str() );

    if( FreeLibrary( m_controlLib ) == 0 )
    {
        LOG_ERROR( "FreeLibrary() call failed. Error %d", GetLastError() );
    }

    m_controlLib = 0;
    m_createModule = NULL;
    m_releaseModule = NULL;
    m_loadedDllName.clear();
}

PM_MODULE_RESULT_T UCIDLoader::CreateModule( PM_MODULE_CTX_T* pPM_MODULE_CTX )
{
    return m_createModule( pPM_MODULE_CTX );
}

PM_MODULE_RESULT_T UCIDLoader::ReleaseModule( PM_MODULE_CTX_T* pPM_MODULE_CTX )
{
    return m_releaseModule( pPM_MODULE_CTX );
}

void UCIDLoader::LoadControlModule()
{
    if( m_isModuleLoaded )
    {
        WLOG_ERROR( L"UnifiedConnectorID Control Module already running." );
        return;
    }

    std::wstring ucidDllDir;
    std::wstring dllFullPath;
    if( !HelperFunctions::ReadRegistryString( HKEY_LOCAL_MACHINE, L"Software\\Cisco\\SecureXYZ\\UnifiedConnector\\UCID", L"Path", ucidDllDir ) )
    {
        WLOG_ERROR( L"Failed to read UnifiedConnectorID Control Module data from registry" );
        return;
    }

    dllFullPath = ucidDllDir;
    if( HelperFunctions::Is64BitWindows() )
    {
        dllFullPath.append( L"x64\\" );
    }
    else
    {
        dllFullPath.append( L"x86\\" );
    }
    dllFullPath.append( L"ucidcontrolplugin.dll" );

    std::wstring ucidConfigFile( ucidDllDir );
    ucidConfigFile.append( UCID_MODULE_CONFIG_FILENAME );

    if( !HelperFunctions::FileExists( ucidConfigFile.c_str() ) )
    {
        WLOG_ERROR( L"UnifiedConnectorID Control Module configuration file not found: %s", ucidConfigFile.c_str() );
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
        WLOG_ERROR( "Exception: %s", ex.what() );
    }

    m_context.nVersion = UCID_MODULE_INTERFACE_VERSION;
    if( m_context.fpInit )
    {
        m_context.fpInit();
    }

    PM_MODULE_RESULT_T result;

    if( ( result = CreateModule( &m_context ) ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to load UnifiedConnectorID Control Module: CreateModuleInstance() returned %d.", result );
        return;
    }

    //HACK to load UCID file. Revet when fix in UCID!!!!!
    ucidConfigFile = L"\"" + ucidConfigFile + L"\"";
    if( ( result = m_context.fpStart( ucidDllDir.c_str(), ucidDllDir.c_str(), ucidConfigFile.c_str() ) ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to start UnifiedConnectorID Control Module: fpStart() returned %d.", result );
        return;
    }

    m_isModuleLoaded = true;

    WLOG_DEBUG( L"UnifiedConnectorID Control Module loaded and started." );
}

void UCIDLoader::UnloadControlModule()
{
    if( !m_isModuleLoaded )
    {
        WLOG_ERROR( L"UnifiedConnectorID Control Module already released." );
        return;
    }

    PM_MODULE_RESULT_T result;

    if( ( result = m_context.fpStop() ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to stop UnifiedConnectorID Control Module: fpStop() returned %d.", result );
        return;
    }

    if( ( result = ReleaseModule( &m_context ) ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to release UnifiedConnectorID Control Module: ReleaseModuleInstance() returned %d.", result );
        return;
    }

    m_isModuleLoaded = false;

    WLOG_DEBUG( L"UnifiedConnectorID Control Module stopped and released." );
}

#include "stdafx.h"

#include "UCMCPLoader.h"
#include "IUCLogger.h"
#include "ICodeSignVerifier.h"

UCMCPLoader::UCMCPLoader( ICodesignVerifier& codeSignVerifier )
    : m_codeSignVerifier( codeSignVerifier )
    , m_controlLib( 0 )
    , m_createModule( NULL )
    , m_releaseModule( NULL )
    , m_loadedDllName()
{
}

UCMCPLoader::~UCMCPLoader()
{
    UnloadDll();
}

bool UCMCPLoader::LoadDll( const std::wstring dllPath )
{
    std::wstring dllDir;

    std::wstring::size_type pos = dllPath.find_last_of( L"/\\" );
    if( pos == std::wstring::npos || pos >= dllPath.size() )
    {
        WLOG_ERROR( L"Invalid dll path specified: %s", dllPath.c_str());
        return false;
    }

    dllDir = dllPath.substr( 0, pos );
    m_loadedDllName = dllPath.substr( pos + 1, dllPath.size() - pos -1 );

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
    if( !m_controlLib  ) {
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
        throw std::exception("Couldn't bind to ReleaseModuleInstance dll function. Error %d", GetLastError() );
    }

    return true;
}

void UCMCPLoader::UnloadDll()
{
    if( !m_controlLib )
    {
        return;
    }

    LOG_DEBUG( "UnLoading %s", m_loadedDllName.c_str() );

    if( FreeLibrary( m_controlLib  ) == 0 )
    {
        LOG_ERROR( "FreeLibrary() call failed. Error %d", GetLastError() );
    }

    m_controlLib = 0;
    m_createModule = NULL;
    m_releaseModule = NULL;
    m_loadedDllName.clear();
}

PM_MODULE_RESULT_T UCMCPLoader::CreateModule( PM_MODULE_CTX_T* pPM_MODULE_CTX, IUcLogger* logger )
{
    return m_createModule( pPM_MODULE_CTX , logger );
}

PM_MODULE_RESULT_T UCMCPLoader::ReleaseModule( PM_MODULE_CTX_T* pPM_MODULE_CTX )
{
    return m_releaseModule( pPM_MODULE_CTX );
}

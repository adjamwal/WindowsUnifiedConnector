// libPmAgent.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "PmAgent.h"
#include "IPackageManager.h"
#include "IPmLogger.h"
#include "IUcLogger.h"
#include <codecvt>

PmAgent::PmAgent( const std::wstring& bsConfigFilePath, const std::wstring& pmConfigFilePath, IPmPlatformDependencies& dependencies, IPMLogger& pmLogger ) :
    m_pmConfigFile( pmConfigFilePath )
    , m_pmBootstrapFile( bsConfigFilePath )
    , m_pmDependencies( dependencies )
    , m_pmLogger( pmLogger )
    , m_PacMan( nullptr )

{
    SetPMLogger( &pmLogger );
    InitPackageManager();
    m_PacMan = GetPackageManagerInstance();
    m_PacMan->SetPlatformDependencies( &m_pmDependencies );
}

PmAgent::~PmAgent()
{
    DeinitPackageManager();
    SetPMLogger( nullptr );
}

int32_t PmAgent::Start()
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string pmConfigFile = converter.to_bytes( m_pmConfigFile );
    std::string pmBootstrapFile = converter.to_bytes( m_pmBootstrapFile );
    
    LOG_DEBUG( "Starting Package Manager Client with config files %s", pmConfigFile.c_str() );
    return m_PacMan->Start( pmConfigFile.c_str(), pmBootstrapFile.c_str() );
}

int32_t PmAgent::Stop()
{
    LOG_DEBUG( "Stopping Package Manager Client" );
    return m_PacMan->Stop();
}

int32_t PmAgent::VerifyPmConfig( const std::wstring& pmConfigFilePath )
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string pmConfigFile = converter.to_bytes( m_pmConfigFile );

    LOG_DEBUG( "Verifing config file %s", pmConfigFile.c_str() );
    return m_PacMan->VerifyPmConfig( pmConfigFile.c_str() );
}
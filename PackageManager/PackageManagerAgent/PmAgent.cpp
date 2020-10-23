// PackageManagerAgent.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "PmAgent.h"
#include "IPackageManager.h"
#include "IPmLogger.h"
#include "IUcLogger.h"
#include <codecvt>

PmAgent::PmAgent( const std::wstring& configFilePath, IPmPlatformDependencies& dependencies, IPMLogger& pmLogger ) :
    m_configFile( configFilePath )
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
    std::string configFile = converter.to_bytes( m_configFile );
    
    LOG_DEBUG( "Staring Package Manager Client with config file %s", configFile.c_str() );
    return m_PacMan->Start( configFile.c_str() );
}

int32_t PmAgent::Stop()
{
    LOG_DEBUG( "Stopping Package Manager Client" );
    return m_PacMan->Stop();
}

int32_t PmAgent::VerifyConfig( const std::wstring& configFilePath )
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string configFile = converter.to_bytes( configFilePath );

    LOG_DEBUG( "Verifing config file %s", configFile.c_str() );
    return m_PacMan->VerifyPacManConfig( configFile.c_str() );
}
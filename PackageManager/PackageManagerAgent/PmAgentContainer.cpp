#include "pch.h"

#include "PmAgentContainer.h"
#include "WindowsConfiguraton.h"
#include "WindowsComponentManager.h"
#include "WindowsPmDependencies.h"
#include "PmAgent.h"
#include "PmLogAdapter.h"
#include "IUcLogger.h"
#include "WinCertLoader.h"

PmAgentContainer::PmAgentContainer( const std::wstring& configFilePath ) :
    m_certLoader( new WinCertLoader() )
    , m_configuration( new WindowsConfiguration( *m_certLoader ) )
    , m_componentMgr( new WindowsComponentManager() )
    , m_pmDependencies( new WindowsPmDependencies( *m_configuration, *m_componentMgr ) )
    , m_pmLogger( new PmLogAdapter() )
    , m_pmAgent( new PmAgent( configFilePath, *m_pmDependencies, *m_pmLogger ) )
{
}

PmAgentContainer::~PmAgentContainer()
{
}

PmAgent& PmAgentContainer::pmAgent()
{
    return *m_pmAgent;
}


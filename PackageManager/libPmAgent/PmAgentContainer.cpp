#include "pch.h"

#include "PmAgentContainer.h"
#include "WindowsConfiguration.h"
#include "WindowsComponentManager.h"
#include "WindowsPmDependencies.h"
#include "PmAgent.h"
#include "PmLogAdapter.h"
#include "IUcLogger.h"
#include "CodesignVerifier.h"
#include "PackageDiscoveryMethods.h"
#include "PackageDiscovery.h"
#include "WinCertLoader.h"
#include "WinApiWrapper.h"

PmAgentContainer::PmAgentContainer( const std::wstring& bsConfigFilePath, const std::wstring& pmConfigFilePath ) :
    m_winApiWrapper( new WinApiWrapper() )
    , m_certLoader( new WinCertLoader() )
    , m_codeSignVerifer( new CodesignVerifier() )
    , m_discoveryMethods( new PackageDiscoveryMethods() )
    , m_packageDiscovery( new PackageDiscovery( *m_discoveryMethods ) )
    , m_configuration( new WindowsConfiguration( *m_certLoader, *m_codeSignVerifer ) )
    , m_componentMgr( new WindowsComponentManager( *m_winApiWrapper, *m_codeSignVerifer, *m_packageDiscovery ) )
    , m_pmDependencies( new WindowsPmDependencies( *m_configuration, *m_componentMgr ) )
    , m_pmLogger( new PmLogAdapter() )
    , m_pmAgent( new PmAgent( bsConfigFilePath, pmConfigFilePath, *m_pmDependencies, *m_pmLogger ) )
{
}

PmAgentContainer::~PmAgentContainer()
{
}

PmAgent& PmAgentContainer::pmAgent()
{
    return *m_pmAgent;
}


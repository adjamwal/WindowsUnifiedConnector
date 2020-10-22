#include "CertsAdapter.h"
#include "PmLogger.h"
#include "PmHttp.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformConfiguration.h"

CertsAdapter::CertsAdapter()
    : m_dependencies( nullptr )
    , m_certList( { 0 } )
{
}

CertsAdapter::~CertsAdapter()
{
    if( m_dependencies ) {
        m_dependencies->Configuration().ReleaseSslCertificates( m_certList.certificates, m_certList.count );
    }
}

void CertsAdapter::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    //can't allow overwriting if m_certList was already allocated
    if( m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Already initialized." );
    }

    m_dependencies = dep;
    m_dependencies->Configuration().GetSslCertificates( &m_certList.certificates, m_certList.count );
}

PmHttpCertList CertsAdapter::GetCertsList()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    return m_certList;
}

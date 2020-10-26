#include "CertsAdapter.h"
#include "PmLogger.h"
#include "PmHttp.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformConfiguration.h"

CertsAdapter::CertsAdapter()
    : m_dependencies( nullptr )
    , m_certList( { 0 } )
    , m_hasCerts( false )
{
}

CertsAdapter::~CertsAdapter()
{
    InternalReleaseCerts();
}

void CertsAdapter::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    //release any previously allocated certs
    InternalReleaseCerts();

    m_dependencies = dep;
    InternalGetCerts();
}

PmHttpCertList CertsAdapter::GetCertsList()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    return m_certList;
}

void CertsAdapter::InternalGetCerts()
{
    if( !m_dependencies || m_hasCerts ) return;

    try
    {
        LOG_DEBUG( "Obtaining SSL certs" );
        m_dependencies->Configuration().GetSslCertificates( &m_certList.certificates, m_certList.count );
        m_hasCerts = true;
    }
    catch( ... )
    {
        LOG_ERROR( "Error getting SSL certs." );
    }
}

void CertsAdapter::InternalReleaseCerts()
{
    if( !m_dependencies || !m_hasCerts ) return;
    m_hasCerts = false;

    try
    {
        LOG_DEBUG( "Releasing SSL certs" );
        m_dependencies->Configuration().ReleaseSslCertificates( m_certList.certificates, m_certList.count );
    }
    catch( ... )
    {
        LOG_ERROR( "Error releasing SSL certs." );
    }
}
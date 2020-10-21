#include "TokenAdapter.h"
#include "PmLogger.h"
#include "PmHttp.h"

TokenAdapter::TokenAdapter()
    : m_dependencies( nullptr )
{
}

TokenAdapter::~TokenAdapter()
{
}

void TokenAdapter::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_dependencies = dep;
}

std::string TokenAdapter::GetUcidToken()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    if( m_dependencies->Configuration().GetIdentityToken( m_token ) )
    {
        LOG_DEBUG( "GetIdentityToken: %s", m_token.c_str() );
    }
    else
    {
        LOG_ERROR( "Failed to get new token; using last known token for requests." );
    }

    return m_token;
}
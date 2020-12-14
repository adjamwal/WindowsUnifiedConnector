#include "UcidAdapter.h"
#include "PmLogger.h"
#include "PmHttp.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformConfiguration.h"

UcidAdapter::UcidAdapter()
    : m_dependencies( nullptr )
{
}

UcidAdapter::~UcidAdapter()
{
}

void UcidAdapter::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_dependencies = dep;
}

std::string UcidAdapter::GetAccessToken()
{
    std::lock_guard<std::mutex> lock( m_mutex );
    std::string token;

    if( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    if( !m_dependencies->Configuration().GetIdentityToken( token ) ) {
        LOG_ERROR( "Failed to get token" );
    }

    return token;
}

std::string UcidAdapter::GetIdentity()
{
    std::lock_guard<std::mutex> lock( m_mutex );
    std::string ucid;

    if ( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    if ( !m_dependencies->Configuration().GetUcIdentity( ucid ) ) {
        LOG_ERROR( "Failed to get ucid" );
    }

    return ucid;
}

bool UcidAdapter::Refresh()
{
    std::lock_guard<std::mutex> lock( m_mutex );
    bool rtn = false;

    if ( !m_dependencies ) {
        throw std::exception( __FUNCTION__ ": Dependencies not initialized." );
    }

    rtn = m_dependencies->Configuration().RefreshIdentity();
    if ( !rtn ) {
        LOG_ERROR( "Failed to refresh ucid" );
    }

    return rtn;
}
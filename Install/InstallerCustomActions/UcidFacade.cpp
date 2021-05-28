#include "stdafx.h"
#include "UcidFacade.h"

UcidFacade::UcidFacade()
    : m_oldLogger( GetUcLogger() )
    , m_certLoader( new WinCertLoader() )
    , m_codeSignVerifer( new CodesignVerifier() )
    , m_winConf( new WindowsConfiguration( *m_certLoader, *m_codeSignVerifer ) )
{
}

UcidFacade::~UcidFacade()
{
}

bool UcidFacade::FetchCredentials( std::string& ucid, std::string& ucidToken )
{
    bool retval = false;
    try
    {
        m_winConf->RefreshIdentity();
        m_winConf->GetUcIdentity( ucid );
        m_winConf->GetIdentityToken( ucidToken );

        retval = true;
    }
    catch( std::exception& ex )
    {
        LOG_ERROR( __FUNCTION__ ": Exception caught: %s", ex.what() );
    }
    catch( ... )
    {
        LOG_ERROR( __FUNCTION__ ": Unknown exception caught" );
    }

    return retval;
}

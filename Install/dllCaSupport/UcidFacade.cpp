#include "pch.h"
#include "UcidFacade.h"
#include "WinCertLoader.h"
#include "CodesignVerifier.h"
#include "WindowsConfiguration.h"
#include "IPmConfig.h"
#include <WindowsUtilities.h>
#include <codecvt>
#include "ProxyContainer.h"

UcidFacade::UcidFacade()
    : m_oldLogger( GetUcLogger() )
    , m_certLoader( new WinCertLoader() )
    , m_codeSignVerifer( new CodesignVerifier() )
    , m_proxyContainer( new ProxyContainer() )
    , m_winConf( new WindowsConfiguration( *m_certLoader, *m_codeSignVerifer, m_proxyContainer->GetProxyDiscovery() ) )
{
}

UcidFacade::~UcidFacade()
{
}

bool UcidFacade::CollectUCData( std::string& url, std::string& ucid, std::string& ucidToken )
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    bool retval = false;

    try
    {
        PmUrlList urlList;

        retval = m_winConf->RefreshIdentity() &&
                 m_winConf->GetUcIdentity( ucid ) &&
                 m_winConf->GetIdentityToken( ucidToken ) &&
                 m_winConf->GetPmUrls( urlList );

        //RD - 08/18/2021
        // Shortcut to append the version to the URL. PMConfig does this for us, however including PMConfig in this facade means
        // we need to create it's dependencies (FileSysUtil, UcidAdapter, new Fake PmDependencies/PmComponentManager classes )
        // seems like a lot of work to append a string
        url = urlList.eventUrl + EVENT_URL_VERSION;
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

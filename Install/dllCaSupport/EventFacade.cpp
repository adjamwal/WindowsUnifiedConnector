#include "pch.h"
#include "EventFacade.h"
#include "FileSysUtil.h"
#include "PmHttp.h"
#include "PmCloud.h"
#include "InstallerConfig.h"
#include "MsiCloudEventStorage.h"
#include "CloudEventPublisher.h"
#include "WinCertLoader.h"
#include "CodesignVerifier.h"
#include "WindowsConfiguration.h"
#include "CloudEventBuilder.h"
#include "PmConstants.h"
#include "Utf8PathVerifier.h"
#include "PmLogAdapter.h"
#include "ProxyContainer.h"

EventFacade::EventFacade()
    : m_pmLogger( new PmLogAdapter() )
    , m_utf8PathVerifier( new Utf8PathVerifier() )
    , m_fileUtil( new FileSysUtil( *m_utf8PathVerifier ) )
    , m_http( new PmHttp( *m_fileUtil ) )
    , m_cloud( new PmCloud( *m_http ) )
    , m_config( new InstallerConfig() )
    , m_eventStorage( new MsiCloudEventStorage() )
    , m_certLoader( new WinCertLoader() )
    , m_codeSignVerifer( new CodesignVerifier() )
    , m_proxyContainer( new ProxyContainer() )
    , m_winConf( new WindowsConfiguration( *m_certLoader, *m_codeSignVerifer, m_proxyContainer->GetProxyDiscovery() ) )
    , m_eventPublisher( new CloudEventPublisher( *m_cloud, *m_eventStorage, *m_config ) )
{
    SetPMLogger( m_pmLogger.get() );
}

EventFacade::~EventFacade()
{
    SetPMLogger( NULL );
}

bool EventFacade::SendEventOnUninstallBegin( std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken )
{
    bool retval = false;

    try
    {
        CloudEventBuilder ev;
        ev.WithUCID( ucid );
        ev.WithPackageID( UC_PACKAGE_NAME + std::string("/") + productVersion );
        ev.WithType( CloudEventType::pkguninstall );

        retval = SendEvent( ev, url, ucid, ucidToken );
    }
    catch ( std::exception& ex )
    {
        LOG_ERROR( "Exception caught: %s", ex.what() );
    }
    catch ( ... )
    {
        LOG_ERROR( "Unknown exception caught" );
    }

    return retval;
}

bool EventFacade::SendEventOnUninstallError( std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken )
{
    bool retval = false;

    try
    {
        CloudEventBuilder ev;
        ev.WithUCID( ucid );
        ev.WithPackageID( UC_PACKAGE_NAME + std::string( "/" ) + productVersion );
        ev.WithType( CloudEventType::pkguninstall );
        ev.WithError( -1, "Uninstall Error, no error code available" );

        retval = SendEvent( ev, url, ucid, ucidToken );
    }
    catch ( std::exception& ex )
    {
        LOG_ERROR( "Exception caught: %s", ex.what() );
    }
    catch ( ... )
    {
        LOG_ERROR( "Unknown exception caught" );
    }

    return retval;
}

bool EventFacade::SendEventOnUninstallComplete( std::string& url, std::string& productVersion, std::string& ucid, std::string& ucidToken )
{
    bool retval = false;

    try
    {
        CloudEventBuilder ev;
        ev.WithUCID( ucid );
        ev.WithPackageID( UC_PACKAGE_NAME + std::string( "/" ) + productVersion );
        ev.WithType( CloudEventType::pkguninstall );
        retval = SendEvent( ev, url, ucid, ucidToken );
    }
    catch ( std::exception& ex )
    {
        LOG_ERROR( "Exception caught: %s", ex.what() );
    }
    catch ( ... )
    {
        LOG_ERROR( "Unknown exception caught" );
    }

    return retval;
}

bool EventFacade::SendEvent( ICloudEventBuilder& ev, std::string& url, std::string& ucid, std::string& ucidToken )
{
    bool retval = true;
    PmHttpCertList certList{ 0 };

    m_config->SetCloudEventUri( url );
    
    m_winConf->GetSslCertificates( &certList.certificates, certList.count );

    m_cloud->SetCerts( certList );

    m_eventPublisher->SetToken( ucidToken );

    int32_t httpStatus = m_eventPublisher->Publish( ev );

    LOG_DEBUG( "SendEvent Publish status: %d", httpStatus );

    return retval;
}

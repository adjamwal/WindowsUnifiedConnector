#include "pch.h"
#include "UcidFacade.h"
#include "FileSysUtil.h"
#include "PmConfig.h"
#include "WinCertLoader.h"
#include "CodesignVerifier.h"
#include "WindowsConfiguration.h"
#include <WindowsUtilities.h>
#include <codecvt>
#include "Utf8PathVerifier.h"

UcidFacade::UcidFacade()
    : m_oldLogger( GetUcLogger() )
    , m_utf8PathVerifier( new Utf8PathVerifier() )
    , m_fileUtil( new FileSysUtil( *m_utf8PathVerifier ) )
    , m_config( new PmConfig( *m_fileUtil ) )
    , m_certLoader( new WinCertLoader() )
    , m_codeSignVerifer( new CodesignVerifier() )
    , m_winConf( new WindowsConfiguration( *m_certLoader, *m_codeSignVerifer ) )
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
        std::wstring bsConfigFileW;

        if ( !WindowsUtilities::ReadRegistryString(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Cisco\\SecureClient\\UnifiedConnector\\config",
            L"Bootstrapper",
            bsConfigFileW ) )
        {
            LOG_ERROR( __FUNCTION__ ": Failed to get bsConfigFile" );

            return false;
        }

        m_config->LoadBsConfig( converter.to_bytes( bsConfigFileW ) );
        url = m_config->GetCloudEventUri();

        retval = m_winConf->RefreshIdentity() &&
                 m_winConf->GetUcIdentity( ucid ) &&
                 m_winConf->GetIdentityToken( ucidToken );
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

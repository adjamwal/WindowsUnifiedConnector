#include "PackageManagerContainer.h"
#include "PmTypes.h"
#include "PackageManager.h"
#include "PmCloud.h"
#include "PmHttp.h"
#include "PmConfig.h"
#include "PmManifest.h"
#include "WorkerThread.h"

#include "PackageInventoryProvider.h"
#include "PackageDiscoveryManager.h"
#include "CheckinFormatter.h"
#include "UcidAdapter.h"
#include "CertsAdapter.h"
#include "CatalogListRetriever.h"
#include "CheckinManifestRetriever.h"
#include "ComponentPackageProcessor.h"
#include "ManifestProcessor.h"
#include "PackageConfigProcessor.h"
#include "CloudEventStorage.h"
#include "CloudEventBuilder.h"
#include "CloudEventPublisher.h"
#include "UcUpgradeEventHandler.h"
#include "CatalogJsonParser.h"

#include "FileSysUtil.h"
#include "SslUtil.h"
#include "PmLogger.h"
#include "PmConstants.h"
#include "InstallerCacheManager.h"
#include "RebootHandler.h"
#include "Utf8PathVerifier.h"
#include "Watchdog.h"

#include <mutex>
#include <exception>
#include <curl/curl.h>

static std::mutex gContainerMutex;
static PackageManagerContainer* gContainer = NULL;

PackageManagerContainer::PackageManagerContainer() :
    m_utfPathVerifier( new Utf8PathVerifier() )
    , m_fileUtil( new FileSysUtil( *m_utfPathVerifier ) )
    , m_sslUtil( new SslUtil() )
    , m_http( new PmHttp( *m_fileUtil ) )
    , m_cloud( new PmCloud( *m_http ) )
    , m_installeracheMgr( new InstallerCacheManager( *m_cloud, *m_fileUtil, *m_sslUtil ) )
    , m_config( new PmConfig( *m_fileUtil ) )
    , m_manifest( new PmManifest() )
    , m_thread( new WorkerThread() )
    , m_packageInventoryProvider( new PackageInventoryProvider( *m_fileUtil, *m_sslUtil ) )
    , m_checkinFormatter( new CheckinFormatter() )
    , m_ucidAdapter( new UcidAdapter() )
    , m_certsAdapter( new CertsAdapter() )
    , m_eventStorage( new CloudEventStorage( CLOUD_EVENT_STORAGE_FILENAME, *m_fileUtil, *m_config ) )
    , m_eventBuilder( new CloudEventBuilder() )
    , m_eventPublisher( new CloudEventPublisher( *m_cloud, *m_eventStorage, *m_config ) )
    , m_ucUpgradeEventBuilder( new CloudEventBuilder)
    , m_ucUpgradeEventStorage( new CloudEventStorage( UC_UPGRADE_EVENT_STORAGE_FILENAME, *m_fileUtil, *m_config ) )
    , m_ucUpgradeEventHandler( new UcUpgradeEventHandler( *m_eventPublisher, *m_ucUpgradeEventStorage, *m_ucUpgradeEventBuilder ) )
    , m_checkinManifestRetriever( new CheckinManifestRetriever( *m_cloud, *m_ucidAdapter, *m_certsAdapter ) )
    , m_catalogJsonParser( new CatalogJsonParser() )
    , m_catalogListRetriever( new CatalogListRetriever( *m_cloud, *m_ucidAdapter, *m_certsAdapter, *m_config ) )
    , m_packageDiscoveryManager( new PackageDiscoveryManager( *m_catalogListRetriever, *m_packageInventoryProvider, *m_catalogJsonParser ) )
    , m_packageConfigProcessor( new PackageConfigProcessor( *m_fileUtil, *m_sslUtil, *m_ucidAdapter, *m_eventBuilder, *m_eventPublisher ) )
    , m_watchdog( new Watchdog() )
    , m_componentPackageProcessor( 
        new ComponentPackageProcessor( *m_installeracheMgr,
            *m_fileUtil, 
            *m_sslUtil, 
            *m_packageConfigProcessor, 
            *m_ucidAdapter, 
            *m_eventBuilder, 
            *m_eventPublisher,
            *m_ucUpgradeEventHandler,
            *m_watchdog ) )
    , m_manifestProcessor( new ManifestProcessor( *m_manifest, *m_componentPackageProcessor ) )
    , m_rebootHandler( new RebootHandler( *m_config ) )
    , m_pacMan(
        new PackageManager( *m_config,
            *m_cloud,
            *m_installeracheMgr,
            *m_packageDiscoveryManager,
            *m_checkinFormatter,
            *m_catalogJsonParser,
            *m_ucidAdapter,
            *m_certsAdapter,
            *m_checkinManifestRetriever,
            *m_manifestProcessor,
            *m_eventPublisher,
            *m_eventStorage,
            *m_ucUpgradeEventHandler,
            *m_rebootHandler,
            *m_thread,
            *m_watchdog ) )
{
    curl_global_init( CURL_GLOBAL_DEFAULT );
}

PackageManagerContainer::~PackageManagerContainer()
{
    curl_global_cleanup();
}

IPackageManager& PackageManagerContainer::packageManager()
{
    return *m_pacMan;
}

int32_t InitPackageManager()
{
    int32_t rtn = -1;
    std::lock_guard<std::mutex> lock( gContainerMutex );

    if( gContainer ) {
        LOG_ERROR( "Package Manager already Initialized" );
    }
    else {
        LOG_DEBUG( "Initializing Package Manager" );

        try {
            gContainer = new PackageManagerContainer();
        }
        catch( std::exception& e ) {
            LOG_ERROR( "Failed to initialize Package Manager: %s", e.what() );
            gContainer = NULL;
        }
    }

    return gContainer != NULL;
}

int32_t DeinitPackageManager()
{
    std::lock_guard<std::mutex> lock( gContainerMutex );

    if( !gContainer ) {
        LOG_ERROR( "Package Manager was not Initialized" );
    }
    else {
        delete gContainer;
        gContainer = NULL;

        LOG_DEBUG( "Deinitialized Package Manager" );
    }

    return gContainer == NULL;
}

IPackageManager* GetPackageManagerInstance()
{
    std::lock_guard<std::mutex> lock( gContainerMutex );

    if( gContainer ) {
        return &gContainer->packageManager();
    }

    LOG_ERROR( "Package Manager was not Initialized" );
    return NULL;
}
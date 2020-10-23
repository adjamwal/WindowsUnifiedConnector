#include "PackageManagerContainer.h"
#include "PackageManager.h"
#include "PmCloud.h"
#include "PmHttp.h"
#include "PmConfig.h"
#include "PmManifest.h"
#include "WorkerThread.h"

#include "PackageInventoryProvider.h"
#include "CheckinFormatter.h"
#include "TokenAdapter.h"
#include "CertsAdapter.h"
#include "CheckinManifestRetriever.h"
#include "ComponentPackageProcessor.h"
#include "ManifestProcessor.h"

#include "FileUtil.h"
#include "SslUtil.h"
#include "PmLogger.h"

#include <mutex>
#include <exception>
#include <curl/curl.h>

static std::mutex gContainerMutex;
static PackageManagerContainer* gContainer = NULL;

PackageManagerContainer::PackageManagerContainer() :
    m_fileUtil( new FileUtil() )
    , m_sslUtil( new SslUtil() )
    , m_http( new PmHttp( *m_fileUtil ) )
    , m_cloud( new PmCloud( *m_http ) )
    , m_config( new PmConfig( *m_fileUtil ) )
    , m_manifest( new PmManifest() )
    , m_thread( new WorkerThread() )
    , m_packageInventoryProvider( new PackageInventoryProvider() )
    , m_checkinFormatter( new CheckinFormatter() )
    , m_tokenAdapter( new TokenAdapter() )
    , m_certsAdapter( new CertsAdapter() )
    , m_checkinManifestRetriever( new CheckinManifestRetriever( *m_cloud, *m_tokenAdapter, *m_certsAdapter ) )
    , m_componentPackageProcessor( new ComponentPackageProcessor( *m_cloud, *m_fileUtil, *m_sslUtil ) )
    , m_manifestProcessor( new ManifestProcessor( *m_manifest, *m_componentPackageProcessor ) )
    , m_pacMan(
        new PackageManager( *m_config,
            *m_packageInventoryProvider,
            *m_checkinFormatter,
            *m_tokenAdapter,
            *m_certsAdapter,
            *m_checkinManifestRetriever,
            *m_manifestProcessor,
            *m_thread ) )
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
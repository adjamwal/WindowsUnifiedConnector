#include "PackageManagerContainer.h"
#include "PackageManager.h"
#include "PmCloud.h"
#include "PmConfig.h"
#include "PmManifest.h"
#include "WorkerThread.h"

#include "FileUtil.h"
#include "PmLogger.h"

#include <mutex>
#include <exception>

static std::mutex gContainerMutex;
static PackageManagerContainer* gContainer = NULL;

PackageManagerContainer::PackageManagerContainer() :
    m_fileUtil( new FileUtil() )
    , m_cloud ( new PmCloud() )
    , m_config( new PmConfig( *m_fileUtil ) )
    , m_manifest( new PmManifest() )
    , m_thread( new WorkerThread() )
    , m_pacMan( new PackageManager( *m_config, *m_cloud, *m_manifest, *m_thread ) )
{

}

PackageManagerContainer::~PackageManagerContainer()
{

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
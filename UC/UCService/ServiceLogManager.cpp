#include "ServiceLogManager.h"

#include "UcLogFile.h"
#include "UcLogger.h"
#include "UcConfig.h"

ServiceLogManager::ServiceLogManager() :
    m_config( new UcConfig() )
    , m_logFile( nullptr )
    , m_logger( nullptr )
    , m_running( false )
{

}

ServiceLogManager::~ServiceLogManager()
{
    Stop();
}

void ServiceLogManager::Start()
{
    m_logFile = std::unique_ptr<IUcLogFile>( new UcLogFile() );
    m_logFile->Init( NULL ); //log file name is generated here
    m_logger = std::unique_ptr<UcLogger>( new UcLogger( *m_logFile ) );
    UpdateLogLevel();

    SetUcLogger( m_logger.get() );

    m_running = true;
    m_thread = std::thread( &ServiceLogManager::UpdateLogLevelThread, this );
}

void ServiceLogManager::Stop()
{
    if( m_running ) {
        m_running = false;
        m_cv.notify_one();
        m_thread.join();
    }

    m_logger.reset();
    m_logFile.reset();
}

void ServiceLogManager::UpdateLogLevel()
{
    m_config->LoadConfig();
    m_logger->SetLogLevel( ( IUcLogger::Severity )m_config->GetLogLevel() );
}

void ServiceLogManager::UpdateLogLevelThread()
{
    std::unique_lock<std::mutex> lock( m_mutex );
    WLOG_DEBUG( L"ThreadStarted" );

    while( m_running ) {
        if( m_cv.wait_for( lock, std::chrono::seconds( 2 ), [this] { return !m_running; } ) ) {
            WLOG_DEBUG( L"Shutdown requested" );
            break;
        }

        UpdateLogLevel();
    }

    WLOG_DEBUG( L"Exiting Thread" );
}
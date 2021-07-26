#include "Watchdog.h"
#include "PmLogger.h"

Watchdog::Watchdog() :
    m_enabled( false )
{
#if defined( _WIN32 )
    InitializeCriticalSection( &m_waitLock );
    InitializeConditionVariable( &m_threadCond );
#endif
}

Watchdog::~Watchdog()
{
    Stop();
#if defined( _WIN32 )
    DeleteCriticalSection( &m_waitLock );
#endif
}

void Watchdog::Start( std::function< std::chrono::milliseconds() >waitFunc, std::function<void()> timeoutFunc )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    if( !m_enabled ) {
        m_enabled = true;
        m_thread = std::thread( [this, waitFunc, timeoutFunc]()
            {
                while( m_enabled ) {
#if defined( _WIN32 )
                    EnterCriticalSection( &m_waitLock );

                    DWORD timeout = ( DWORD )waitFunc().count();
                    
                    if( SleepConditionVariableCS( &m_threadCond, &m_waitLock, timeout ) ) {
                        if( !m_enabled ) {
                            LeaveCriticalSection( &m_waitLock );
                            break;
                        }
                    }
                    else {
                        if( GetLastError() == ERROR_TIMEOUT ) {
                            timeoutFunc();
                        }
                    }

                    LeaveCriticalSection( &m_waitLock );
#else
                    std::unique_lock<std::mutex> lock( m_waitLock );

                    // This was a concern in the first windows implementation. wait_for translates the wait time to
                    // an absolute value. If a user rolls their clock back one hour it will make this call wait for
                    // and additional hour. Not sure if this affects mac/linux
                    if( m_threadCond.wait_for( lock, waitFunc() ) == std::cv_status::timeout ) {
                        timeoutFunc();
                    }
#endif
                }
            } );
    }
}

void Watchdog::Stop()
{
    std::lock_guard<std::mutex> lock( m_mutex );
    if( m_enabled ) {
        m_enabled = false;
#if defined( _WIN32 )
        WakeConditionVariable( &m_threadCond );
#else
        m_threadCond.notify_one();
#endif
        m_thread.join();
    }
}

void Watchdog::Kick()
{
    LOG_DEBUG( "Enter" );
#if defined( _WIN32 )
    WakeConditionVariable( &m_threadCond );
#else
    m_threadCond.notify_one();
#endif
}
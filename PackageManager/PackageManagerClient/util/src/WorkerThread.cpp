#include "WorkerThread.h"

WorkerThread::WorkerThread() :
    m_enabled( false )
{
#if defined( _WIN32 )
    InitializeCriticalSection( &m_waitLock );
    InitializeConditionVariable( &m_threadCond );
#endif
}

WorkerThread::~WorkerThread()
{
    Stop();
#if defined( _WIN32 )
    DeleteCriticalSection( &m_waitLock );
#endif
}

void WorkerThread::Start( std::function< std::chrono::milliseconds() >waitFunc, std::function<void()> workFunc )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    if( !m_enabled ) {
        m_enabled = true;
        m_thread = std::thread( [this, waitFunc, workFunc]()
            {
                while( m_enabled ) {
#if defined( _WIN32 )
                    EnterCriticalSection( &m_waitLock );

                    SleepConditionVariableCS( &m_threadCond, &m_waitLock, ( DWORD )waitFunc().count() );

                    if( !m_enabled ) {
                        LeaveCriticalSection( &m_waitLock );
                        break;
                    }

                    workFunc();

                    LeaveCriticalSection( &m_waitLock );
#else
                    std::unique_lock<std::mutex> lock( m_waitLock );

                    // This was a concern in the first windows implementation. wait_for translates the wait time to
                    // an absolute value. If a user rolls their clock back one hour it will make this call wait for
                    // and additional hour. Not sure if this affects mac/linux
                    if( m_threadCond.wait_for( lock, waitFunc(), [this]() { return !m_enabled; } ) ) {
                        break;
                    }

                    workFunc();
#endif
                }
            } );
    }
}

void WorkerThread::Stop()
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

bool WorkerThread::IsRunning()
{
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_enabled;
}
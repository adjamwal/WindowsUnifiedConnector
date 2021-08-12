#include "gtest/gtest.h"
#include "Watchdog.h"

#include <memory>

class ComponentTestWatchdog : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_patient.reset( new Watchdog() );
    }

    void TearDown()
    {
        m_patient.reset();
    }

    std::unique_ptr<Watchdog> m_patient;
    std::mutex m_mutex;
    std::mutex m_configMutex;
    std::condition_variable m_cv;
    std::chrono::milliseconds m_waitTimeMs;

public:
    std::chrono::milliseconds WaitFunc()
    {
        return m_waitTimeMs;
    }
};

TEST_F( ComponentTestWatchdog, DISABLED_WatchdogWillFireAfterTimeout )
{
    bool pass = false;
    
    m_waitTimeMs = std::chrono::milliseconds( 500 );
    
    auto testStart = std::chrono::system_clock::now();
    m_patient->Start( std::bind( &ComponentTestWatchdog::WaitFunc, this ), [this, &pass]() 
        { 
            pass = true;
            m_cv.notify_one();
        } );

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    auto testEnd= std::chrono::system_clock::now();
    lock.unlock();
    
    EXPECT_TRUE( pass );
    EXPECT_GE( ( std::chrono::time_point_cast< std::chrono::milliseconds >( testEnd ) - std::chrono::time_point_cast< std::chrono::milliseconds >( testStart ) ).count(), 500 );
}

TEST_F( ComponentTestWatchdog, WatchdogWontFireIfKicked )
{
    m_waitTimeMs = std::chrono::milliseconds( 500 );

    m_patient->Start( std::bind( &ComponentTestWatchdog::WaitFunc, this ), [this]()
        {
            ASSERT_TRUE( false );
            m_cv.notify_one();
        } );

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::milliseconds( 250 ) );
    lock.unlock();

    m_patient->Kick();

    lock.lock();
    m_cv.wait_for( lock, std::chrono::milliseconds( 350 ) );
    lock.unlock();
}

TEST_F( ComponentTestWatchdog, StoppingWatchdogDoesntTriggerTimeout )
{
    m_waitTimeMs = std::chrono::milliseconds( 500 );

    m_patient->Start( std::bind( &ComponentTestWatchdog::WaitFunc, this ), [this]()
        {
            ASSERT_TRUE( false );
            m_cv.notify_one();
        } );

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::milliseconds( 250 ) );
    lock.unlock();

    m_patient->Stop();

    lock.lock();
    m_cv.wait_for( lock, std::chrono::milliseconds( 750 ) );
    lock.unlock();
}
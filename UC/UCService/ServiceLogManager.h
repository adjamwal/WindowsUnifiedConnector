#pragma once

#include <memory>
#include <thread>
#include <condition_variable>
#include <mutex>

class IUcLogFile;
class UcLogger;
class IUcConfig;

class ServiceLogManager
{
public:
    ServiceLogManager();
    ~ServiceLogManager();

    void Start();
    void Stop();

private:
    std::unique_ptr<IUcConfig> m_config;
    std::unique_ptr<IUcLogFile> m_logFile;
    std::unique_ptr<UcLogger> m_logger;
    std::thread m_thread;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    bool m_running;

    void UpdateLogLevel();
    void UpdateLogLevelThread();
};

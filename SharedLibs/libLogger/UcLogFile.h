#pragma once

#include "IUcLogFile.h"
#include <string>
#include <mutex>
#include <filesystem>

class UcLogFile : public IUcLogFile
{
public:
    UcLogFile();
    ~UcLogFile();

    void WriteLogLine( const char* logLevel, const char* logLine ) override;
    void WriteLogLine( const wchar_t* logLevel, const wchar_t* logLine ) override;

    void Init( const wchar_t* logname = NULL ) override;

    void SetLogConfig( uint32_t fileSize, uint32_t logFiles ) override;

    void Deinit();

private:
    std::filesystem::path m_logFileName;
    std::mutex m_mutex;
    FILE* m_file;
    uint32_t m_lastTick;
    uint32_t m_maxFileSize;
    uint32_t m_maxLogFiles;

    static const uint32_t DEFAULT_MAX_FILE_SIZE = 52428800;
    static const uint32_t DEFAULT_MAX_LOGFILES = 10;

    std::filesystem::path GenerateFileName( const wchar_t* logname );
    bool CreateLogFile();
    void RotateLogs();
    void CleanLogs();
    std::wstring GetProgramDataFolder();
};

#include "pch.h"
#include "UcLogFile.h"
#include <Windows.h>
#include <processenv.h>
#include <set>
#include <initguid.h>
#include <KnownFolders.h>
#include <ShlObj.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

#define UC_REG_KEY L"SOFTWARE\\Cisco\\SecureClient\\Cloud Management"

#if UC_CONSUMER == 1
#define UC_DEFAULT_LOG_PATH L"\\Cisco\\Immunet\\UC"
#else
#define UC_DEFAULT_LOG_PATH L"\\Cisco\\SecureClient\\UC"
#endif

UcLogFile::UcLogFile() :
    m_logFileName( L"" )
    , m_lastTick( 0 )
    , m_maxFileSize( DEFAULT_MAX_FILE_SIZE )
    , m_maxLogFiles( DEFAULT_MAX_LOGFILES )
    , m_loggerName ("rot_log")
{
    class trace_formatter : public spdlog::custom_flag_formatter
    {
    public:
        void format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest) override
        {
            std::string tickStr = std::to_string(GetTickCount64());
            dest.append(tickStr.data(), tickStr.data() + tickStr.size());
        }

        std::unique_ptr< custom_flag_formatter > clone() const override
        {
            return spdlog::details::make_unique< trace_formatter >();
        }
    };

    auto formatter = std::make_unique< spdlog::pattern_formatter >();
    formatter->add_flag< trace_formatter >('*').set_pattern("(%*, +%o ms) %b %d %T [%t] %v");
    spdlog::set_formatter(std::move(formatter));
}

UcLogFile::~UcLogFile()
{
    Deinit();
}

void UcLogFile::WriteLogLine( const char* logLevel, const char* logLine )
{
    Init(); // Takes mutex

    std::string strLevel(logLevel);
    std::string strLog(logLine);

    if (IsLoggerInitialized()) {
        spdlog::get(m_loggerName)->info(strLevel + ": " + strLog);
        Flush();
    }
    else {
        spdlog::error("failed to log ({})", strLog);
    }
}

void UcLogFile::WriteLogLine(const wchar_t* logLevel, const wchar_t* logLine)
{
    Init(); // Takes mutex

    std::string strLevel(ConvertWCHARToString(logLevel));
    std::string strLog(ConvertWCHARToString(logLine));

    if (IsLoggerInitialized()) {
        spdlog::get(m_loggerName)->info(strLevel + ": " + strLog);
        Flush();
    }
    else {
        spdlog::error("failed to log ({})", strLog);
    }
}

void UcLogFile::Init(const wchar_t* logname)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!IsLoggerInitialized()) {
        m_logFileName = GenerateFileName(logname);

        CreateLogFile();
    }
}

void UcLogFile::SetLogConfig(uint32_t fileSize, uint32_t logFiles)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxFileSize = fileSize;
    m_maxLogFiles = logFiles;

    if (IsLoggerInitialized() && std::filesystem::exists(m_logFileName.parent_path())) {
        DropLogger();
        spdlog::rotating_logger_mt(m_loggerName, m_logFileName.string(), m_maxFileSize, m_maxLogFiles - 1);
    }

}

void UcLogFile::Deinit()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (IsLoggerInitialized()) {
        DropLogger();
    }
}

bool UcLogFile::IsLoggerInitialized()
{
    return !(spdlog::get(m_loggerName) == NULL);
}

void UcLogFile::DropLogger()
{
    Flush();
    spdlog::drop(m_loggerName);
}

void UcLogFile::Flush()
{
    if (IsLoggerInitialized()) {
        spdlog::get(m_loggerName)->flush();
    }
}

std::wstring UcLogFile::GetProgramDataFolder()
{
    PWSTR path = NULL;
    std::wstring programData;

    HRESULT hr = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &path);

    if (SUCCEEDED(hr)) {
        programData = path;
        CoTaskMemFree(path);
        path = NULL;
    }

    return programData;
}

static bool ReadRegistryStringW(HKEY hKey, const std::wstring& subKey, const std::wstring& valueName, std::wstring& data)
{
    DWORD dataSize{};
    LONG retCode = ::RegGetValue(hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, nullptr, &dataSize);

    if (retCode != ERROR_SUCCESS) {
        return false;
    }

    data.resize(dataSize / sizeof(wchar_t));

    retCode = ::RegGetValue(hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, &data[0], &dataSize);
    if (retCode != ERROR_SUCCESS) {
        return false;
    }

    DWORD stringLengthInWchars = dataSize / sizeof(wchar_t);
    stringLengthInWchars--; // Exclude the NUL written by the Win32 API
    data.resize(stringLengthInWchars);

    return true;
}

std::wstring UcLogFile::GetLogDir()
{
    PWSTR path = NULL;
    std::wstring logDir;

    if (!ReadRegistryStringW(HKEY_LOCAL_MACHINE, UC_REG_KEY, L"LogDir", logDir)) {
        logDir = GetProgramDataFolder();
        logDir += UC_DEFAULT_LOG_PATH;
    }

    return logDir;
}

std::filesystem::path UcLogFile::GenerateFileName(const wchar_t* logname)
{
    std::filesystem::path filename;

    if (logname) {
        filename = logname;
    }
    else {
        WCHAR swPath[MAX_PATH + 5] = { 0 };
        DWORD dwSize = GetModuleFileName(NULL, swPath, MAX_PATH);
        std::wstring modulePath = swPath;

        filename = GetLogDir();

        if (dwSize && (modulePath.find_last_of('\\') != std::wstring::npos)) {
            modulePath = modulePath.substr(modulePath.find_last_of('\\') + 1);
            filename /= modulePath;
        }
        else {
            filename /= L"UnknownApplication";
        }

        filename += L".log";
    }

    return filename;
}

bool UcLogFile::CreateLogFile()
{
    bool ret = true;
    std::shared_ptr<spdlog::logger> logInstance = NULL;

    if (!IsLoggerInitialized() && !m_logFileName.empty() ) {
        try
        {
            if ( !std::filesystem::exists( m_logFileName.parent_path() ) ) {
                std::filesystem::create_directories( m_logFileName.parent_path() );
            }
            logInstance = spdlog::rotating_logger_mt(m_loggerName, m_logFileName.string(), m_maxFileSize, m_maxLogFiles - 1);
        }
        catch( ... )
        {
            ret = false;
        }
    }

    return logInstance ? true : false;
}

char* UcLogFile::ConvertWCHARToString(const wchar_t* pOrig)
{
    DWORD dwLen;
    char* pStr = NULL;

    if (!pOrig) return NULL;

    dwLen = WideCharToMultiByte(CP_UTF8, 0, pOrig, -1, NULL, 0, NULL, NULL) + 1;
    pStr = (char*)malloc(dwLen);
    if (!pStr) {
        return NULL;
    }
    WideCharToMultiByte(CP_UTF8, 0, pOrig, -1, pStr, dwLen, NULL, NULL);
    return pStr;
}

#pragma once

#include <cstdarg>
#include <cstring>
#include <cstdlib>

class IUcLogger
{
public:
    enum Severity {
        LOG_ALERT = 1,
        LOG_CRITICAL = 2,
        LOG_ERROR = 3,
        LOG_WARNING = 4,
        LOG_NOTICE = 5,
        LOG_INFO = 6,
        LOG_DEBUG = 7
    };

    virtual ~IUcLogger() {}
    virtual void Log( Severity severity, const char* msgFormatter, ... ) = 0;
    virtual void Log( Severity severity, const wchar_t* msgFormatter, ... ) = 0;
    virtual void Log( Severity severity, const char* msgFormatter, va_list args ) = 0;
    virtual void Log( Severity severity, const wchar_t* msgFormatter, va_list args ) = 0;

    virtual void SetLogLevel( Severity severity ) = 0;
};

IUcLogger* GetUcLogger();
void SetUcLogger( IUcLogger* logger );

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG_ALERT( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_ALERT, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_ALERT( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_ALERT, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_CRITICAL( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_CRITICAL, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_CRITICAL( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_CRITICAL, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_ERROR( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_ERROR, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_ERROR( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_ERROR, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_WARNING( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_WARNING, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_WARNING( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_WARNING, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_NOTICE( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_NOTICE, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_NOTICE( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_NOTICE, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_INFO( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_INFO, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_INFO( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_INFO, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_DEBUG( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_DEBUG, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_DEBUG( fmt, ... ) GetUcLogger()->Log( IUcLogger::LOG_DEBUG, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
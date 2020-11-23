#pragma once

#include "IPMLogger.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG_EMERGENCY( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_EMERGENCY, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_EMERGENCY( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_EMERGENCY, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_ALERT( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_ALERT, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_ALERT( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_ALERT, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_CRITICAL( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_CRITICAL, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_CRITICAL( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_CRITICAL, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_ERROR( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_ERROR, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_ERROR( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_ERROR, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_WARNING( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_WARNING, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_WARNING( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_WARNING, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_NOTICE( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_NOTICE, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_NOTICE( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_NOTICE, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_INFO( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_INFO, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_INFO( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_INFO, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

#define LOG_DEBUG( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_DEBUG, "%s:%s:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )
#define WLOG_DEBUG( fmt, ... ) GetPMLogger()->Log( IPMLogger::LOG_DEBUG, "%hs:%hs:%d: " fmt, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

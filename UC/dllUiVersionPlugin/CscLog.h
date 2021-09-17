/**************************************************************************
*   Copyright (c) 2021 Cisco Systems, Inc.
*   All Rights Reserved. Cisco Highly Confidential.
***************************************************************************
*
*   File: CscLog.h
*   Date: 07/2021
*
***************************************************************************
*   Declaration of Cisco Secure Client common code logging levels and
*   methods for the Secure Endpoint presentation plugin implementation.
***************************************************************************/

#pragma once

enum LOG_LEVEL
{
    CSCLOG_ERROR,
    CSCLOG_WARN,
    CSCLOG_INFO,
    CSCLOG_TRACE
};

#define __CSCFILENAME__ ( strrchr( __FILE__, '\\' ) ? strrchr( __FILE__, '\\' ) + 1 : __FILE__ )

#define csclogDebugMessage( logLevel, fmt, ... ) _uclogDebugMessage( logLevel, "%s:%s:%d: " fmt, __CSCFILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__ )

void _uclogDebugMessage( LOG_LEVEL logLevel, const char* pFormatString, ... );


#pragma once

#include <stdio.h>

#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_FATAL 5

#define LOG_LEVEL_OFF 6

const char LogLevelChars[] = "TDIWEFO";

#ifdef _DEBUG
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#ifndef LOG_LEVEL
#ifdef _DEBUG
#define LOG_LEVEL LOG_LEVEL_TRACE
#else
#define LOG_LEVEL LOG_LEVEL_OFF
#endif
#endif

#if LOG_LEVEL <= LOG_LEVEL_TRACE
#define LogTrace(fmt, ...) gLog->log(LOG_LEVEL_TRACE, fmt, __VA_ARGS__)
#else
#define LogTrace(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LogDebug(fmt, ...) gLog->log(LOG_LEVEL_DEBUG, fmt, __VA_ARGS__)
#else
#define LogDebug(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LogInfo(fmt, ...) gLog->log(LOG_LEVEL_INFO, fmt, __VA_ARGS__)
#else
#define LogInfo(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
#define LogWarn(fmt, ...) gLog->log(LOG_LEVEL_WARN, fmt, __VA_ARGS__)
#else
#define LogWarn(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LogError(fmt, ...) gLog->log(LOG_LEVEL_ERROR, fmt, __VA_ARGS__)
#else
#define LogError(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_FATAL
#define LogFatal(fmt, ...) gLog->log(LOG_LEVEL_FATAL, fmt, __VA_ARGS__)
#else
#define LogFatal(fmt, ...)
#endif
#if LOG_LEVEL != LOG_LEVEL_OFF
#define LogFunctionName LogDebug(__FUNCTION__);

#define LogFunctionNameLimited                           \
    {                                                    \
        static int LOG_COUNT = 10;                       \
        if (LOG_COUNT >= 0)                              \
        {                                                \
            LogDebug(__FUNCTION__ " (%d)", LOG_COUNT--); \
        }                                                \
    }

#define LogInfoLimited(fmt, ...)                                              \
    {                                                                         \
        static int LOG_COUNT = 10;                                            \
        if (LOG_COUNT >= 0)                                                   \
        {                                                                     \
            gLog->log(LOG_LEVEL_INFO, fmt " (%d)", __VA_ARGS__, LOG_COUNT--); \
        }                                                                     \
    }
#else
#define LogFunctionName
#define LogFunctionNameLimited
#define LogInfoLimited
#endif
class cLog
{
public:
    cLog(const char *name, bool log_time = false, int buf_len = 2048);
    virtual ~cLog();
    void log(int log_level, const char *format, ...);
    void set_log_time(bool is_log) { _is_log_time = is_log; };
    bool is_valid() { return _fp != NULL; };

protected:
    FILE *_fp;
    char *_bufA;
    int _buf_len;
    bool _is_log_time;

    void _log_time();
    void _log(int log_level, const char *s);
};

#if LOG_LEVEL != LOG_LEVEL_OFF
extern cLog *gLog;
// #ifndef DEFAULT_LOG_FILE
//	#define DEFAULT_LOG_FILE _T("3dm.log")
// #endif
#endif

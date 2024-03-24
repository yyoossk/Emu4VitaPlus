#pragma once

#include <stdio.h>
#include <string>

#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_FATAL 5

#define LOG_LEVEL_OFF 6

const char LogLevelChars[] = "TDIWEFO";

#ifdef DEBUG
#define LOG_LEVEL LOG_LEVEL_DEBUG
#else
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#if LOG_LEVEL <= LOG_LEVEL_TRACE
#define LogTrace(fmt, ...) gLog->log(LOG_LEVEL_TRACE, fmt, __VA_ARGS__)
#else
#define LogTrace(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LogDebug(fmt, ...) gLog->log(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
#define LogDebug(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LogInfo(fmt, ...) gLog->log(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#else
#define LogInfo(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
#define LogWarn(fmt, ...) gLog->log(LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#else
#define LogWarn(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LogError(fmt, ...) gLog->log(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#else
#define LogError(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_FATAL
#define LogFatal(fmt, ...) gLog->log(LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)
#else
#define LogFatal(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LogFunctionName LogDebug(__PRETTY_FUNCTION__);

#define LogFunctionNameLimited                                     \
    {                                                              \
        static int LOG_COUNT = 10;                                 \
        if (LOG_COUNT >= 0)                                        \
        {                                                          \
            LogDebug("%s (%d)", __PRETTY_FUNCTION__, LOG_COUNT--); \
        }                                                          \
    }

#define LogInfoLimited(fmt, ...)                              \
    {                                                         \
        static int LOG_COUNT = 10;                            \
        if (LOG_COUNT >= 0)                                   \
        {                                                     \
            LogInfo(fmt " (%d)", ##__VA_ARGS__, LOG_COUNT--); \
        }                                                     \
    }
#else
#define LogFunctionName
#define LogFunctionNameLimited
#define LogInfoLimited
#endif

class cLog
{
public:
    cLog(const char *name, int buf_len = 2048);
    virtual ~cLog();
    void log(int log_level, const char *format, ...);

protected:
    std::string _name;
    char *_bufA;
    int _buf_len;

    void _log(int log_level, const char *s);
};

#if LOG_LEVEL != LOG_LEVEL_OFF
extern cLog *gLog;
#endif

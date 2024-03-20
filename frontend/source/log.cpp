#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <psp2/rtc.h>
#include "log.h"

#if LOG_LEVEL != LOG_LEVEL_OFF
cLog *gLog = NULL;
#endif

cLog::cLog(const char *name, bool log_time, int buf_len)
{
	_fp = fopen(name, "w");
	_bufA = new char[buf_len];
	_buf_len = buf_len;
	_is_log_time = log_time;
}

cLog::~cLog()
{
	fclose(_fp);
	delete[] _bufA;
}

void cLog::log(int log_level, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vsprintf(_bufA, format, args);
	va_end(args);

	_log(log_level, _bufA);
}

void cLog::_log(int log_level, const char *s)
{
	fprintf(_fp, "[%c]", LogLevelChars[log_level]);
	if (_is_log_time)
	{
		_log_time();
	}

	fputs(_bufA, _fp);
	fputc('\n', _fp);

	fflush(_fp);
}

void cLog::_log_time()
{
	SceDateTime time;
	sceRtcGetCurrentClockLocalTime(&time);
	fprintf(_fp, "%02d:%02d:%02d.%03d ", time.hour, time.minute, time.second, time.microsecond / 1000);
}
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <psp2/io/fcntl.h>
#include <psp2/rtc.h>
#include "log.h"

#if LOG_LEVEL != LOG_LEVEL_OFF
cLog *gLog = NULL;
#endif

cLog::cLog(const char *name, int buf_len)
{
	_name = name;
	_bufA = new char[buf_len];
	_buf_len = buf_len;
}

cLog::~cLog()
{
	delete[] _bufA;
}

void cLog::log(int log_level, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vsnprintf(_bufA, _buf_len, format, args);
	va_end(args);

	_log(log_level, _bufA);
}

void cLog::_log(int log_level, const char *s)
{
	SceDateTime time;
	sceRtcGetCurrentClockLocalTime(&time);

	FILE *fp = fopen(_name.c_str(), "a");
	if (fp)
	{
		fprintf(fp, "[%c] %02d:%02d:%02d.%03d %s\n", LogLevelChars[log_level], time.hour, time.minute, time.second, time.microsecond / 1000, s);
		fclose(fp);
	}
}

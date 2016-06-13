#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>

#include "log.h"

#define LOG_BUFF_SIZE   1024

static int log_level = LOG_DEBUG;

void log_printf(int level, const char *string, ...)
{
	va_list arg;
	char buffer[LOG_BUFF_SIZE];

	va_start(arg, string);
	vsprintf(buffer, string, arg);
	va_end(arg);

	if (level < LOG_DEBUG)
		syslog(level, "%s", buffer);

	if (level <= log_level) {
		if (level <= LOG_ERR) {
			fprintf(stderr, "%s", buffer);
		} else {
			fprintf(stdout, "%s", buffer);
			fflush(stdout);
		}
	}
}

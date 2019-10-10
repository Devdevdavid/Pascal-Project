#ifndef LOG_H
#define LOG_H

#include "Arduino.h"
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#define LOG_VERSION "0.1.0s"
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define log_trace(...) log_log(LOG_TRACE, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  __FILENAME__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN,  __FILENAME__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILENAME__, __LINE__, __VA_ARGS__)

void log_log(int level, const char *file, int line, const char *fmt, ...);

#endif
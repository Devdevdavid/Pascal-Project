/**
  * @file   log.cpp
  * @brief  Print logs on serial debug port
  * @author David DEVANT
  * @date   12/08/2017
  */

#include "log.hpp"
#include "global.hpp"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef MODULE_TERM
#include "cmd/term.hpp"
#endif

static char         message[128];
static char         argsString[512];
static const char * level_names[] = {
	"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

// C Declaration is needed because log_raw() is
// used in cli_config.h wich is compiled with C Compiler
extern "C" {
void log_raw(const char * fmt, ...)
{
	va_list args;

	/* Log to console */
	va_start(args, fmt);
	vsprintf(argsString, fmt, args);
	va_end(args);

	sprintf(message, "%s", argsString);

#ifdef MODULE_TERM
	term_print(message);
#endif
}
}

void log_log(int level, const char * file, int line, const char * fmt, ...)
{
	va_list args;

	/* Log to console */
	va_start(args, fmt);
	vsprintf(argsString, fmt, args);
	va_end(args);

	sprintf(message, "%-5s [%s]:%d: %s\n", level_names[level], file, line, argsString);

#ifdef MODULE_TERM
	term_print(message);
#endif
}

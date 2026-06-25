#ifndef CSVI_LOG_H_
#define CSVI_LOG_H_

#include <stdbool.h>
#include <stdarg.h>

void csvi_log_set_verbose(bool enabled);
bool csvi_log_is_verbose(void);
void csvi_log_info(const char *fmt, ...);
void csvi_log_error(const char *fmt, ...);

#endif

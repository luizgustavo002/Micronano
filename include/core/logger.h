#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

typedef enum
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

void logger_init(const char *file_name, int mode);
void log_message(LogLevel level, const char *format, ...);
void logger_close(void);

#endif
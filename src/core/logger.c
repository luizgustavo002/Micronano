#include "logger.h"
#include "types_errors.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <dirent.h>
#define CREATE_DIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define CREATE_DIR(path) mkdir(path, 0755)
#endif

static FILE *log_file = NULL;
static int debug_mode = 0;
const char *level_names[] = {"DEBUG", "INFO", "WARN", "ERROR"};

void logger_init(const char *file_name, int mode) {
    if (log_file != NULL) return;
    CREATE_DIR("logs");

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "logs/%s", file_name);

    log_file = fopen(file_path, "a");
    if (!log_file) perror("Error opening log file.");

    debug_mode = mode;
}

void log_message(LogLevel level, const char *format, ...) {
    if (log_file == NULL) return;

    if (debug_mode == 0 && level == LOG_DEBUG) return;

    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    fprintf(log_file, "[%s] [%s] ", timestamp, level_names[level]);

    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);

    fprintf(log_file, "\n");
    fflush(log_file);
}

void logger_close() {
    if (log_file) fclose(log_file);
    log_file = NULL;
}

#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static FILE *log_file = NULL;
static LogLevel current_level = LOG_ERROR;

static const char *level_str(LogLevel l) {
    switch(l) {
        case LOG_ERROR:   return "ERROR";
        case LOG_WARNING: return "WARNING";
        case LOG_INFO:    return "INFO";
        default: return "UNKNOWN";
    }
}

void set_log_level(LogLevel level) {
    current_level = level;
}

void log_message(LogLevel level, const char *format, ...) {
    if (level < current_level) return;
    if (!log_file) {
        log_file = fopen("trainer.log", "a");
        if (!log_file) return;
    }
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log_file, "[%02d:%02d:%02d] [%s] ", t->tm_hour, t->tm_min, t->tm_sec, level_str(level));
    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    fprintf(log_file, "\n");
    fflush(log_file);
}

void close_logger(void) {
    if (log_file) fclose(log_file);
}
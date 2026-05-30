#ifndef LOGGER_H
#define LOGGER_H

typedef enum { LOG_ERROR, LOG_WARNING, LOG_INFO } LogLevel;

void log_message(LogLevel level, const char *format, ...);
void set_log_level(LogLevel level);
void close_logger(void);

#endif
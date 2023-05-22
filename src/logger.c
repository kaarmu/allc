#include "../include/logger.h"

#include <stdarg.h>

// Logger {{{

void allc_logger_log(Logger *self, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(self->stream, fmt, args);
    fprintf(self->stream, "\n");
    va_end(args);
}

void allc_logger_log_info(Logger *self, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(self->stream, "[INFO] ");
    vfprintf(self->stream, fmt, args);
    fprintf(self->stream, "\n");
    va_end(args);
}

void allc_logger_log_warn(Logger *self, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(self->stream, "[WARN] ");
    vfprintf(self->stream, fmt, args);
    fprintf(self->stream, "\n");
    va_end(args);
}

void allc_logger_log_erro(Logger *self, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(self->stream, "[ERRO] ");
    vfprintf(self->stream, fmt, args);
    fprintf(self->stream, "\n");
    va_end(args);
}

void allc_logger_panic(Logger *self, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(self->stream, "[ERRO] ");
    vfprintf(self->stream, fmt, args);
    fprintf(self->stream, "\n");
    va_end(args);
    exit(1);
}

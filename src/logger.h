/*
 *  STB-style header library of a simple logger implementation.
 *
 *  Description
 *  ===========
 *
 *  Provides
 *  ========
 *
 *  Logger
 *
 *  Options
 *  =======
 *
 *  Library
 *  =======
 *
 *  Dependencies:
 *    - allocator.h
 *
 *  Version: 0.0.0
 *
 *  Author: Kaj Munhoz Arfvidsson
 */

#ifndef ALLC_LOGGER_GUARD // {{{1
#define ALLC_LOGGER_GUARD

#include <stdlib.h>
#include <stdio.h>

#include "allocator.h"

// Logger {{{2

typedef struct {
    FILE *stream;
    Allocator allocator;
} Logger;

/*
 * The log* family simply outputs to `self->stream` in a similar fashion to fprintf.
 */
void allc_logger_log(Logger *self, const char *fmt, ...);
void allc_logger_log_info(Logger *self, const char *fmt, ...);
void allc_logger_log_warn(Logger *self, const char *fmt, ...);
void allc_logger_log_erro(Logger *self, const char *fmt, ...);

/*
 * Print and exit.
 */
void allc_logger_panic(Logger *self, const char *fmt, ...);

#endif // ALLC_LOGGER_GUARD }}}1

#ifdef ALLC_IMPL_LOGGER // {{{1
#ifdef ALLC_IMPL_DEPENDENCIES
#   define ALLC_IMPL_ALLOCATOR
#endif
#ifndef ALLC_IMPL_LOGGER_GUARD
#define ALLC_IMPL_LOGGER_GUARD

#include <stdarg.h>

#include "allocator.h"

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
    fprintf(self->stream, "[INFO]");
    vfprintf(self->stream, fmt, args);
    fprintf(self->stream, "\n");
    va_end(args);
}

void allc_logger_log_warn(Logger *self, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(self->stream, "[WARN]");
    vfprintf(self->stream, fmt, args);
    fprintf(self->stream, "\n");
    va_end(args);
}

void allc_logger_log_erro(Logger *self, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(self->stream, "[ERRO]");
    vfprintf(self->stream, fmt, args);
    fprintf(self->stream, "\n");
    va_end(args);
}

void allc_logger_panic(Logger *self, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(self->stream, "[ERRO]");
    vfprintf(self->stream, fmt, args);
    fprintf(self->stream, "\n");
    va_end(args);
    exit(1);
}

#endif // ALLC_IMPL_LOGGER_GUARD
#endif // ALLC_IMPL_LOGGER }}}1

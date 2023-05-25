/*
 *  Simple logger.
 *
 *  Description
 *  ===========
 *
 *  Options
 *  =======
 *
 *  Content
 *  =======
 *
 *  1) Logger
 *
 *      [x] Logger
 *      [x] allc_logger_log
 *      [x] allc_logger_log_info
 *      [x] allc_logger_log_warn
 *      [x] allc_logger_log_erro
 *      [x] allc_logger_panic
 */

#ifndef ALLC_LOGGER__GUARD
#define ALLC_LOGGER__GUARD

// Includes {{{1
// -------------

#include "../allocator/allocator.h"

#include <stdlib.h>
#include <stdio.h>


// Logger {{{1
// -----------

typedef struct Logger {
    FILE *stream;
    Allocator allocator;
} Logger;

/**
 * The log* family simply outputs to `self->stream` in a similar fashion
 * to fprintf.
 **/
void allc_logger_log(Logger *self, const char *fmt, ...);
void allc_logger_log_info(Logger *self, const char *fmt, ...);
void allc_logger_log_warn(Logger *self, const char *fmt, ...);
void allc_logger_log_erro(Logger *self, const char *fmt, ...);

/*
 * Print and exit with return code 1.
 */
void allc_logger_panic(Logger *self, const char *fmt, ...);


#endif // ALLC_LOGGER__GUARD

/*
 *  Simple logger.
 *
 *  Description
 *  ===========
 *  << What is this module about? >>
 *
 *  Options
 *  =======
 *  << What compilation options is available? >>
 *
 *  Content
 *  =======
 *  << What types/subroutines does this module provide? >>
 *
 *  Logger
 *  ------
 *  [x] Logger
 *  [x] allc_logger_log
 *  [x] allc_logger_log_info
 *  [x] allc_logger_log_warn
 *  [x] allc_logger_log_erro
 *  [x] allc_logger_panic
 */

#ifndef ALLC_LOGGER_GUARD
#define ALLC_LOGGER_GUARD

#include <stdlib.h>
#include <stdio.h>

#include "allocator.h"

// Logger {{{

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
 * Print and exit with return code 1.
 */
void allc_logger_panic(Logger *self, const char *fmt, ...);

#endif // ALLC_LOGGER_GUARD

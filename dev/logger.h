/***

Simple logger.

Description
===========

Options
=======

Authored by Kaj Munhoz Arfvidsson, 2023.

***/

#ifndef ALLC_LOGGER__GUARD
#define ALLC_LOGGER__GUARD

// Includes {{{1
// =============

#include <stdio.h>
#include <stdlib.h>

#include "allocator.h"

// Logger {{{1
// ===========

struct logger_s {
  FILE *stream;
};

typedef struct logger_s Logger;

// Logger - Constructing and Desctructing {{{2
// -------------------------------------------

Logger allc_logger_new_stdout();

Logger allc_logger_new_stderr();

// Logger - Log {{{2
// -----------------

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

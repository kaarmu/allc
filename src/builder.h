/*
 *  STB-style header library of tools for building your own project-specific build system.
 *
 *  Heavily inspired by https://github.com/tsoding/nobuild.
 *
 *  Provides
 *  ========
 *
 *  Builder
 *
 *  Options
 *  =======
 *
 *  Package
 *  =======
 *
 *  Depends:
 *    - allocator.h
 *    - logger.h
 *    - strbuf.h
 *    - path.h
 *    - list.h
 *
 *  Author: Kaj Munhoz Arfvidsson, 2023
 */

#ifndef ALLC_BUILDER_GUARD // {{{1
#define ALLC_BUILDER_GUARD

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#include "allocator.h"
#include "logger.h"
#include "strbuf.h"
#include "path.h"
#include "list.h"

// StrBufList {{{2

typedef
    ALLC_LIST_ITEM_TYPE(StrBuf)
    StrBufListItem;

List allc_cstr_split(StrBuf *self, const char *sep);
List allc_strbuf_split(StrBuf *self, const char *sep);

// Builder {{{2

typedef struct {
    Logger logger;
    Allocator allocator;
} Builder;

/*
 * Create a new builder object.
 */
Builder allc_builder_new()
{
    Allocator allocator = allc_allocator();
    Logger logger {
        .stream = stderr,
        .allocator = allocator,
    };
    return Builder {
        .logger = logger,
        .allocator = allocator,
    };
};

// Call external commands {{{3

/*
 * Call command synchronously.
 */
void allc_builder_exec(Builder *self, const char *command)
{
    pid_t cpid = fork();
    if (cpid < 0) {
        allc_logger_panic(&self->logger,
                          "(%s) Could not fork a child process with command \"%s\".",
                          command,
                          strerror(errno));
    }
    if (cpid == 0) {
        const char *cmd; // TODO
        char *const *args;
        if (execvp(cmd, args) < 0) {
            allc_logger_panic(&self->logger,
                              "(%s) Could not execute child process with command \"%s\".",
                              command,
                              strerror(errno));
        }
        return;
    }
    while (true) {
        int status;
        if (waitpid(cpid , &status , WNOHANG) < 0) {
            allc_logger_panic(&self->logger,
                              "An error occurred while waiting for a child process to complete.");
        }
        if (WIFSIGNALED(status) != 0) {
            allc_logger_panic(&self->logger,
                              "The command \"%s\" was terminated by %s.",
                              command,
                              strsignal(WTERMSIG(status)));
        }
        if (WIFEXITED(status) == 0) {
            continue;
        }
        if (WEXITSTATUS(status) != 0) {
            allc_logger_panic(&self->logger,
                              "The command \"%s\" exited with exit code %d.",
                              command,
                              WEXITSTATUS(status));
        }
    }
}

#endif // ALLC_BUILDER_GUARD }}}1

// Stuff
// =====
//
// FOREACH_ARRAY
//
// cstr_ends_with
// ENDS_WITH
//
// cstr_no_ext
// NOEXT
//
// Cstr_Array
// cstr_array_make
// cstr_array_append
// cstr_array_join
// JOIN
// CONCAT
// PATH
//
// Pipe { Fd read; Fd write }
// Cmd
//
// fd_open_for_read
// fd_open_for_write
// fd_close
// pid_wait
// cmd_show
// cmd_run_async
//
// Cmd_Array
//
// CMD
//
// Chain_Token_Type
// Chain_Token_Type
//
// IN
// OUT
// CHAIN_CMD
//
// Chain
//
// REBUILD_URSELF
// GO_REBUILD_URSELF
//
// rebuild_urself
//
// path_is_dir
// path_exists
// path_mkdirs
// path_rename
// path_rm
//
// FOREACH_FILE_IN_DIR
//
// NOBUILD_PRINTF_FORMAT
//
// VLOG
// INFO
// WARN
// ERRO
// PANIC
//
// shift_args



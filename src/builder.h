/*
 *  STB-style header library of tools for building your own project-specific
 *  build system.
 *
 *  Heavily inspired by https://github.com/tsoding/nobuild.
 *
 *  Description
 *  ===========
 *
 *  Provides
 *  ========
 *
 *  Builder
 *
 *  Options
 *  =======
 *
 *  Library
 *  =======
 *
 *  Dependencies:
 *    - allocator.h
 *    - logger.h
 *    - strbuf.h
 *    - path.h
 *    - list.h
 *
 *  Version: 0.0.0
 *
 *  Author: Kaj Munhoz Arfvidsson, 2023
 */

// Stuff from nobuild
// ==================
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

// Builder {{{2

typedef struct {
    Logger logger;
    Allocator allocator;
} Builder;

#endif // ALLC_BUILDER_GUARD }}}1

#ifdef ALLC_IMPL_BUILDER // {{{1
#ifdef ALLC_IMPL_DEPENDENCIES
#   define ALLC_IMPL_ALLOCATOR
#   define ALLC_IMPL_LOGGER
#   define ALLC_IMPL_STRBUF
#   define ALLC_IMPL_PATH
#   define ALLC_IMPL_LIST
#endif
#ifndef ALLC_IMPL_BUILDER__GUARD
#define ALLC_IMPL_BUILDER__GUARD

#include "allocator.h"
#include "logger.h"
#include "strbuf.h"
#include "path.h"
#include "list.h"



#endif // ALLC_IMPL_BUILDER__GUARD
#endif // ALLC_IMPL_BUILDER }}}1


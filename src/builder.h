/*
 *  Build your own project-specific build system.
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

// Builder

typedef struct {
    Logger logger;
    Allocator allocator;
} Builder;

#endif // ALLC_BUILDER_GUARD

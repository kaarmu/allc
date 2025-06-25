/*
 * Build your own project-specific build system.
 *
 * Description
 * ===========
 * << What is this module about? >>
 *
 * Options
 * =======
 * << What compilation options is available? >>
 *
 * Todo
 * ====
 * - [ ] path_is_dir
 * - [ ] path_exists
 * - [ ] path_mkdirs
 * - [ ] path_rename
 * - [ ] path_rm
 * 
 * Authored by Kaj Munhoz Arfvidsson, 2023.
 */

#ifndef ALLC_BUILDER__GUARD
#define ALLC_BUILDER__GUARD

// Includes {{{1
// =============

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "allocator.h"
#include "list.h"
#include "logger.h"
#include "path.h"
#include "strbuf.h"

// Builder {{{1
// ============

typedef struct {
  Path cwd;
  Logger logger;
  allc_allocator_t allocator;
} Builder;

typedef struct builder_s *Builder;

Builder allc_builder_new();

// Builder allc_builder_run();


// Stuff from nobuild
// ==================
//
// cstr_no_ext
// NOEXT
//
// Pipe { Fd read; Fd write }
//
// Cmd
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

#endif // ALLC_BUILDER__GUARD

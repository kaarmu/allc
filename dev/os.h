/***
Kaj Munhoz Arfvidsson, 2023
***/

#ifdef ALLC_IMPL
#   define ALLC_OS_IMPL
#   define ALLC_DIRECTORY_IMPL
#   define ALLC_FILE_IMPL
#   define ALLC_TIME_IMPL
#   define ALLC_ENV_IMPL
#   define ALLC_PROCESS_IMPL
#   define ALLC_MEM_IMPL
#   define ALLC_NET_IMPL
#endif

#include "os/directory.h"   // [x] DIRECTORY OPERATIONS
#include "os/file.h"        // [x] FILE OPERATIONS
#include "os/time.h"        // [o] TIME MANAGEMENT
#include "os/environ.h"     // [ ] ENVIROMENT VARIABLES
#include "os/process.h"     // [o] PROCESS MANAGEMENT
#include "os/memory.h"      // [ ] MEMORY MANAGEMENT
#include "os/network.h"     // [ ] NETWORKING


#ifndef ALLC_OS__GUARD
#define ALLC_OS__GUARD

struct allc_os_call_arg_s {
    allc_allocator_t allocator;
    String command;
};

enum allc_os_call_status_e {};

void allc_os_call(struct allc_os_call_arg_s *args);

#endif // ALLC_OS__GUARD

#ifndef ALLC_OS_IMPL__GUARD
#   define ALLC_OS_IMPL__GUARD
#else
#   undef ALLC_OS_IMPL
#endif

#ifdef ALLC_OS_IMPL // {{{1

// int allc_os_call(allc_allocator_t allocator, String command)
// {
//     int result = -1;
//     List args = allc_list_new(allocator);

//     struct allc_strbuf_pair_s pair;
//     pair.right = allc_strbuf_new_from_cstr(allocator, command);
//     while (pair.right->length > 0) {
//         pair = allc_strbuf_split_on_char(pair.right, 1, ' ');
//         allc_list_append(&args, sizeof(StrBuf), &pair.left);
//     }

//     return_(allc_process_spawn_wait(&args));

// defer:
//     ALLC_LIST_FOREACH(StrBuf, elem, &args, {
//         allc_strbuf_delete(*elem);
//     });  
//     allc_list_delete(args);
//     return result;
// }

#endif // ALLC_OS_IMPL // }}}1

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker

/**
 *  Useful pre-processor macros.
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
 *  1) Macros
 *
 *      [x] ALLC_PANIC
 *      [x] ALLC_NOT_IMPLEMENTED
 *      [x] ALLC_DEBUG_VALUE
 *
 *  (Authored by Kaj Munhoz Arfvidsson, 2023)
 **/

#ifndef ALLC_MACRO
#define ALLC_MACRO

// Includes {{{1
// -------------

#include "cstr.h"
#include "strbuf/strbuf.h"
#include <stdio.h>
#include <stdlib.h>


// Macros {{{1
// -----------

#define ALLC_PANIC(msg)                                                      \
    do {                                                                     \
        fprintf(stderr, "(%s:%d)%s: %s", __FILE__, __LINE__, __func__, msg); \
        exit(1);                                                             \
    } while(0)

#define ALLC_NOT_IMPLEMENTED                                                           \
    do {                                                                               \
        fprintf(stderr, "(%s:%d)%s: NOT IMPLEMENTED\n", __FILE__, __LINE__, __func__); \
        exit(1);                                                                       \
    } while(0)

#define ALLC_DEBUG_VALUE(fmt, ...) \
    printf("(L%03d) " fmt, __LINE__, __VA_ARGS__)

#define ALLC_TEST(expected, fmt, actual)                    \
    do {                                                    \
        StrBuf s = allc_strbuf_new(allc_allocator_c());     \
        allc_strbuf_set_fmt(&s, fmt, actual);               \
        if (allc_cstr_is_equal(expected, s.data))           \
            ALLC_DEBUG_VALUE("%s", "true");                 \
        else                                                \
            ALLC_DEBUG_VALUE("%s: \"%s\" / \"" fmt "\"",    \
                             "false", expected, actual);    \
        printf("\n");                                       \
        allc_strbuf_del(&s);                                \
    }while(0)


#endif // ALLC_MACRO

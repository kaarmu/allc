/*
 *  Useful pre-processor macros.
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
 *  Macros
 *  ------
 *  [x] ALLC_PANIC
 *  [x] ALLC_NOT_IMPLEMENTED
 *  [x] ALLC_DEBUG_VALUE
 */

#ifndef ALLC_MACRO
#define ALLC_MACRO

#include <stdio.h>
#include <stdlib.h>

// Macros {{{

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

#define ALLC_DEBUG_VALUE(fmt, value) \
    printf("(L%03d) " fmt "\n", __LINE__, value)

#endif // ALLC_MACRO

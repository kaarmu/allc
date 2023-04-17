/*
 * STB-style header library of pre-processor macros.
 *
 * Author: Kaj Munhoz Arfvidsson
 */

#ifndef ALLC_MACRO
#define ALLC_MACRO

#include <stdio.h>
#include <stdlib.h>

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

#endif // ALLC_MACRO

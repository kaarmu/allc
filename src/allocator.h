/*
 * STB-style header library of allocator objects.
 *
 * Provides:
 *   - Allocator
 *
 * Author: Kaj Munhoz Arfvidsson
 */

#ifndef ALLC_ALLOCATOR_GUARD /// {{{1
#define ALLC_ALLOCATOR_GUARD

#include <stddef.h>
#include <stdlib.h>

// Type Declarations {{{2

typedef struct {
    void* (*alloc)(size_t);
    void* (*realloc)(void*, size_t);
    void (*free)(void*);
} Allocator;

// Allocator management {{{2

Allocator allc_allocator();

#endif // ALLC_ALLOCATOR_GUARD }}}1

#ifdef ALLC_IMPL_ALLOCATOR // {{{1
#ifndef ALLC_IMPL_ALLOCATOR_GUARD
#define ALLC_IMPL_ALLOCATOR_GUARD

// Allocator management {{{2

Allocator allc_allocator() {
    return (Allocator) {
        .alloc = &malloc,
        .realloc = &realloc,
        .free = &free,
    };
}

#endif // ALLC_IMPL_ALLOCATOR_GUARD
#endif // ALLC_IMPL_ALLOCATOR }}}1

/*
 *  Allocators.
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
 *  Allocator Object
 *  ----------------
 *  [x] Allocator
 *  [x] allc_allocator_c
 *
 *  Arena Allocator
 *  ---------------
 *  [ ] ArenaAllocator
 *  [ ] allc_allocator_arena
 */

#ifndef ALLC_ALLOCATOR_GUARD
#define ALLC_ALLOCATOR_GUARD

#include <stddef.h>
#include <stdlib.h>

// Allocator Object

typedef struct {
    void* (*alloc)(size_t);
    void* (*realloc)(void*, size_t);
    void (*free)(void*);
} Allocator;

Allocator allc_allocator_c();

#endif // ALLC_ALLOCATOR_GUARD

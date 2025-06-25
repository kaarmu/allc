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
 *  ALLC_ALLOCATOR_NO8
 *  ALLC_ALLOCATOR_NO16
 *  ALLC_ALLOCATOR_NO32
 *  ALLC_ALLOCATOR_NO64
 *
 */

// Module Dependencies {{{1
// ========================

// AllC {{{2
// ---------

#define ALLC_IMPL // During development

#ifdef ALLC_IMPL
#   define ALLC_ALLOCATOR_IMPL
#endif

// Includes {{{2
// -------------

#include "types.h"

// Macros {{{1
// ===========

#define returnd(value)  \
    do {                \
        result = value; \
        goto defer;     \
    } while (0)

// Declarations {{{1
// =================

#ifndef ALLC_ALLOCATOR_GUARD
#define ALLC_ALLOCATOR_GUARD

#if     ALLC_SYSTEM_SIZE == 8
#   undef ALLC_ALLOCATOR_NO8
#elif   ALLC_SYSTEM_SIZE == 16
#   undef ALLC_ALLOCATOR_NO16
#elif   ALLC_SYSTEM_SIZE == 32
#   undef ALLC_ALLOCATOR_NO32
#elif   ALLC_SYSTEM_SIZE == 64
#   undef ALLC_ALLOCATOR_NO64
#endif

// TODO: Move
typedef struct {
    allc_u8_t size;     // Total size of the buffer
    allc_u8_t used;     // Amount of buffer used
    allc_u8_t memory[]; // Start of buffer itself
} allc_buf8_s;          // Has 8bit = 255 sized of virt. addr. space

typedef struct {
    allc_u16_t size;    // Total size of the buffer
    allc_u16_t used;    // Amount of buffer used
    allc_u8_t memory[]; // Start of buffer itself
} allc_buf16_s;         // Has 16bit = 65536 sized of virt. addr. space

typedef struct {
    allc_u32_t size;    // Total size of the buffer
    allc_u32_t used;    // Amount of buffer used
    allc_u8_t memory[]; // Start of buffer itself
} allc_buf32_s;         // Has 32bit = 2^32 sized of virt. addr. space

typedef struct {
    allc_u64_t size;    // Total size of the buffer
    allc_u64_t used;    // Amount of buffer used
    allc_u8_t memory[]; // Start of buffer itself
} allc_buf64_s;         // Has 64bit = 2^64 sized of virt. addr. space


// Base Allocator {{{2
// -------------------

typedef enum {
    ALLC_ALLOCATOR_sys,
    #ifndef ALLC_ALLOCATOR_NO8
    ALLC_ALLOCATOR_8,
    #endif
    #ifndef ALLC_ALLOCATOR_NO16 
    ALLC_ALLOCATOR_16,
    #endif
    #ifndef ALLC_ALLOCATOR_NO32
    ALLC_ALLOCATOR_32,
    #endif
    #ifndef ALLC_ALLOCATOR_NO64
    ALLC_ALLOCATOR_64,
    #endif
} allc_allocator_kind_e; 

typedef struct allc_allocator {
    allc_allocator_kind_e kind;
    union {
        #ifndef ALLC_ALLOCATOR_NO8
        struct {
            allc_u8_t   (*alloc  )(void *self, allc_u8_t sz);
            allc_u8_t   (*realloc)(void *self, allc_u8_t adr, allc_u8_t sz);
            void        (*free   )(void *self, allc_u8_t adr);
            void*       (*adr2ptr)(void *self, allc_u8_t adr);
            allc_u8_t   (*ptr2adr)(void *self, void *ptr);
        } fn8;
        #endif

        #ifndef ALLC_ALLOCATOR_NO16
        struct {
            allc_u16_t  (*alloc  )(void *self, allc_u16_t sz);
            allc_u16_t  (*realloc)(void *self, allc_u16_t adr, allc_u16_t sz);
            void        (*free   )(void *self, allc_u16_t adr);
            void*       (*adr2ptr)(void *self, allc_u16_t adr);
            allc_u16_t  (*ptr2adr)(void *self, void *ptr);
        } fn16;
        #endif

        #ifndef ALLC_ALLOCATOR_NO32
        struct {
            allc_u32_t  (*alloc  )(void *self, allc_u32_t sz);
            allc_u32_t  (*realloc)(void *self, allc_u32_t ptr, allc_u32_t sz);
            void        (*free   )(void *self, allc_u32_t ptr);
            void*       (*adr2ptr)(void *self, allc_u32_t adr);
            allc_u32_t  (*ptr2adr)(void *self, void *ptr);
        } fn32;
        #endif

        #ifndef ALLC_ALLOCATOR_NO64
        struct {
            allc_u64_t  (*alloc  )(void *self, allc_u64_t sz);
            allc_u64_t  (*realloc)(void *self, allc_u64_t ptr, allc_u64_t sz);
            void        (*free   )(void *self, allc_u64_t ptr);
            void*       (*adr2ptr)(void *self, allc_u64_t adr);
            allc_u64_t  (*ptr2adr)(void *self, void *ptr);
        } fn64;
        #endif
    };
} allc_allocator_s;

void allc_allocator_init(allc_allocator_s *self, allc_allocator_kind_e kind);

#define allc_adr2ptr(self, adr)                                             \
    ((self)->kind == ALLC_ALLOCATOR_8 ? (self)->fn8.adr2ptr(self, adr)   :  \ 
     (self)->kind == ALLC_ALLOCATOR_16 ? (self)->fn16.adr2ptr(self, adr) :  \
     (self)->kind == ALLC_ALLOCATOR_32 ? (self)->fn32.adr2ptr(self, adr) :  \
     (self)->kind == ALLC_ALLOCATOR_64 ? (self)->fn64.adr2ptr(self, adr) :  \
     exit(1))

#define allc_ptr2adr(self, ptr)                                             \
    ((self)->kind == ALLC_ALLOCATOR_8 ? (self)->fn8.ptr2adr(self, ptr)   :  \ 
     (self)->kind == ALLC_ALLOCATOR_16 ? (self)->fn16.ptr2adr(self, ptr) :  \
     (self)->kind == ALLC_ALLOCATOR_32 ? (self)->fn32.ptr2adr(self, ptr) :  \
     (self)->kind == ALLC_ALLOCATOR_64 ? (self)->fn64.ptr2adr(self, ptr) :  \
     exit(1))

#define allc_alloc_fast(self, size)                                         \
    ((self)->kind == ALLC_ALLOCATOR_8 ? (self)->fn8.alloc(self, size)   :   \ 
     (self)->kind == ALLC_ALLOCATOR_16 ? (self)->fn16.alloc(self, size) :   \
     (self)->kind == ALLC_ALLOCATOR_32 ? (self)->fn32.alloc(self, size) :   \
     (self)->kind == ALLC_ALLOCATOR_64 ? (self)->fn64.alloc(self, size) :   \
     exit(1))

inline void *allc_alloc(allc_allocator_s *self, size_t size)
{
    switch (self->kind)
    {
    case ALLC_ALLOCATOR_8:
        if (self->fn8.alloc == 0) return 0;
        allc_u8_t adr = self->fn8.alloc(self, (allc_u8_t) size);
        return self->fn8.adr2ptr(self, adr);
    case ALLC_ALLOCATOR_16:
        if (self->fn16.alloc == 0) return 0;
        allc_u16_t adr = self->fn16.alloc(self, (allc_u16_t) size);
        return self->fn16.adr2ptr(self, adr);
    case ALLC_ALLOCATOR_32:
        if (self->fn32.alloc == 0) return 0;
        allc_u32_t adr = self->fn32.alloc(self, (allc_u32_t) size);
        return self->fn32.adr2ptr(self, adr);
    case ALLC_ALLOCATOR_64:
        if (self->fn64.alloc == 0) return 0;
        allc_u64_t adr = self->fn64.alloc(self, (allc_u64_t) size);
        return self->fn64.adr2ptr(self, adr);
    }
}

#define allc_realloc_fast(self, pointer, size)                                       \
    ((self)->kind == ALLC_ALLOCATOR_8 ? (self)->fn8.realloc(self, pointer, size)   : \ 
     (self)->kind == ALLC_ALLOCATOR_16 ? (self)->fn16.realloc(self, pointer, size) : \
     (self)->kind == ALLC_ALLOCATOR_32 ? (self)->fn32.realloc(self, pointer, size) : \
     (self)->kind == ALLC_ALLOCATOR_64 ? (self)->fn64.realloc(self, pointer, size) : \
     exit(1))

inline void *allc_realloc(allc_allocator_s *self, void *pointer, size_t size)
{
    switch (self->kind)
    {
    case ALLC_ALLOCATOR_8:
        allc_u16_t adr = self->fn8.ptr2adr(self, (allc_u16_t) size);
        return self->fn8.realloc(self, (allc_u8_t) pointer, (allc_u8_t) size);
    case ALLC_ALLOCATOR_16:
        return self->fn16.realloc(self, (allc_u16_t) pointer, (allc_u16_t) size);
    case ALLC_ALLOCATOR_32:
        return self->fn32.realloc(self, (allc_u32_t) pointer, (allc_u32_t) size);
    case ALLC_ALLOCATOR_64:
        return self->fn64.realloc(self, (allc_u64_t) pointer, (allc_u64_t) size);
    }
}

#define allc_free_fast(self, pointer)                                       \
    ((self)->kind == ALLC_ALLOCATOR_8 ? (self)->fn8.free(self, pointer)   : \ 
     (self)->kind == ALLC_ALLOCATOR_16 ? (self)->fn16.free(self, pointer) : \
     (self)->kind == ALLC_ALLOCATOR_32 ? (self)->fn32.free(self, pointer) : \
     (self)->kind == ALLC_ALLOCATOR_64 ? (self)->fn64.free(self, pointer) : \
     exit(1))

inline void allc_free(allc_allocator_s *self, void *pointer)
{
    switch (self->kind)
    {
    case ALLC_ALLOCATOR_8:
        return self->fn8.free(self, (allc_u8_t) pointer);
    case ALLC_ALLOCATOR_16:
        return self->fn16.free(self, (allc_u16_t) pointer);
    case ALLC_ALLOCATOR_32:
        return self->fn32.free(self, (allc_u32_t) pointer);
    case ALLC_ALLOCATOR_64:
        return self->fn64.free(self, (allc_u64_t) pointer);
    }
}

// C Allocator {{{2
// ----------------

typedef struct {
    allc_allocator_s base;
} allc_allocator_c_s;

void allc_allocator_c_init(allc_allocator_c_s *allocator);

// Owned Allocator {{{2
// --------------------

typedef struct {
    allc_allocator_s base;
    allc_allocator_s *owner;
    allc_u8_t alignment;
    union {
        allc_buf_s;
        allc_buf8_s;
        allc_buf16_s;
        allc_buf32_s;
    } buf;
} allc_allocator_owned_s;

allc_allocator_owned_s *allc_allocator_owned_new(allc_allocator_s *owner, allc_allocator_kind_e kind, size_t size);

void  allc_allocator_owned_init(allc_allocator_owned_s *self, allc_allocator_kind_e kind, size_t size);

void  allc_allocator_owned_delete(allc_allocator_owned_s *self);

void *allc_allocator_owned_alloc(allc_allocator_bump_s *self, size_t size, allc_u8_t alignment);


// Bump Allocator {{{2
// -------------------

typedef allc_allocator_owned_s allc_allocator_bump_s;

allc_allocator_bump_s *allc_allocator_bump_new(allc_allocator_kind_e kind, allc_allocator_s *owner, size_t size);

void  allc_allocator_bump_init(allc_allocator_bump_s *self, allc_allocator_kind_e kind, size_t size);

void  allc_allocator_bump_delete(allc_allocator_bump_s *self);

void  allc_allocator_bump_reset(allc_allocator_bump_s *self);

#endif // ALLC_ALLOCATOR_GUARD
// }}}1

/****************************************************************************/

// Definitions {{{1
// ================

#ifndef ALLC_ALLOCATOR_IMPL__GUARD
#   define ALLC_ALLOCATOR_IMPL__GUARD
#else
#   undef ALLC_ALLOCATOR_IMPL
#endif

#ifdef ALLC_ALLOCATOR_IMPL

// Includes {{{2
// -------------

#include <stdlib.h>

// Base Allocator {{{2
// -------------------

void allc_allocator_init(allc_allocator_s *self, allc_allocator_kind_e kind)
{
    *self = (allc_allocator_s) {0};
    self->kind = kind;

    if (self->kind == ALLC_ALLOCATOR_sys) {
        #if     ALLC_SYSTEM_SIZE == 8
        self->kind = ALLC_ALLOCATOR_8;
        #elif   ALLC_SYSTEM_SIZE == 16
        self->kind = ALLC_ALLOCATOR_16;
        #elif   ALLC_SYSTEM_SIZE == 32
        self->kind = ALLC_ALLOCATOR_32;
        #elif   ALLC_SYSTEM_SIZE == 64
        self->kind = ALLC_ALLOCATOR_64;
        #endif
    }
}

// C allocator {{{2
// ----------------

void* allc_allocator_c_alloc_fn(allc_allocator_c_s *self, size_t size)
{
    return malloc(size);
}

void* allc_allocator_c_realloc_fn(allc_allocator_c_s *self, void *pointer, size_t size)
{
    return realloc(pointer, size);
}

void  allc_allocator_c_free_fn(allc_allocator_c_s *self, void *pointer)
{
    return free(pointer);
}

void allc_allocator_c_init(allc_allocator_c_s *self) { 
    allc_allocator_init(self, ALLC_ALLOCATOR_sys);
    self->base.fn64.alloc   = &allc_allocator_c_alloc_fn;
    self->base.fn64.realloc = &allc_allocator_c_realloc_fn;
    self->base.fn64.free    = &allc_allocator_c_free_fn;
}

// Owned Allocator {{{2
// --------------------

// Owned Allocator - 8bit {{{3

allc_u8_t allc_allocator_owned_ptr2adr_fn8(allc_allocator_owned_s *self, void *ptr)
{
    void *base = self->buf.memory;
    return ((size_t) ptr) - ((size_t) base);
}

void *allc_allocator_owned_adr2ptr_fn8(allc_allocator_owned_s *self, allc_u8_t adr)
{
    void *base = self->buf.memory;
    return ((size_t) base) + ((size_t) adr);
}

// Owned Allocator - 16bit {{{3

allc_u16_t allc_allocator_owned_ptr2adr_fn16(allc_allocator_owned_s *self, void *ptr)
{
    void *base = self->buf.memory;
    return ((size_t) ptr) - ((size_t) base);
}

void *allc_allocator_owned_adr2ptr_fn16(allc_allocator_owned_s *self, allc_u16_t adr)
{
    void *base = self->buf.memory;
    return ((size_t) base) + ((size_t) adr);
}

// Owned Allocator - 32bit {{{3

allc_u32_t allc_allocator_owned_ptr2adr_fn32(allc_allocator_owned_s *self, void *ptr)
{
    void *base = self->buf.memory;
    return ((size_t) ptr) - ((size_t) base);
}

void *allc_allocator_owned_adr2ptr_fn32(allc_allocator_owned_s *self, allc_u32_t adr)
{
    void *base = self->buf.memory;
    return ((size_t) base) + ((size_t) adr);
}

// Owned Allocator - 64bit {{{3

allc_u64_t allc_allocator_owned_ptr2adr_fn64(allc_allocator_owned_s *self, void *ptr)
{
    void *base = self->buf.memory;
    return ((size_t) ptr) - ((size_t) base);
}

void *allc_allocator_owned_adr2ptr_fn64(allc_allocator_owned_s *self, allc_u64_t adr)
{
    void *base = self->buf.memory;
    return ((size_t) base) + ((size_t) adr);
}

// Owned Allocator Methods {{{3

#define ALIGN_UP(size, align) (((size) + (align) - 1) & ~((align) - 1))

allc_allocator_owned_s *allc_allocator_owned_new(allc_allocator_s *owner, allc_allocator_kind_e kind, size_t size)
{
    // TODO: Assert size > sizeof(allc_allocator_owned_s)

    // Allocate memory for the owned allocator's structure and its memory block
    allc_allocator_owned_s *self = allc_alloc(owner, size);
    if (!self) {
        // FAILED TO ALLOCATE ALLOCATOR
        return NULL;
    }

    allc_allocator_owned_init(self, kind, size);
    self->owner = owner;

    return self;
}

void allc_allocator_owned_init(allc_allocator_owned_s *self, allc_allocator_kind_e kind, size_t size)
{
    allc_allocator_init(self, kind);
    switch (kind)
    {
    case ALLC_ALLOCATOR_8:
        self->base.fn8.adr2ptr  = allc_allocator_owned_adr2ptr_fn8;
        self->base.fn8.ptr2adr  = allc_allocator_owned_ptr2adr_fn8;
        break;
    case ALLC_ALLOCATOR_16:
        self->base.fn16.adr2ptr = allc_allocator_owned_adr2ptr_fn16;
        self->base.fn16.ptr2adr = allc_allocator_owned_ptr2adr_fn16;
        break;
    case ALLC_ALLOCATOR_32:
        self->base.fn32.adr2ptr = allc_allocator_owned_adr2ptr_fn32;
        self->base.fn32.ptr2adr = allc_allocator_owned_ptr2adr_fn32;
        break;
    case ALLC_ALLOCATOR_64:
        self->base.fn64.adr2ptr  = allc_allocator_owned_adr2ptr_fn64;
        self->base.fn64.ptr2adr  = allc_allocator_owned_ptr2adr_fn64;
        break;
    }

    self->owner = NULL; // We don't know anything about the owner, there might not be one that can be point to (e.g. stack).
 
    self->buf.size = size - sizeof(allc_allocator_bump_s);
    self->buf.used = 0;
}

void allc_allocator_owned_delete(allc_allocator_owned_s *self)
{
    if (self->owner) {
        // Free the owned allocator's memory using the owner allocator
        allc_free(self->owner, self);
    }
}

void *allc_allocator_owned_alloc(allc_allocator_owned_s *self, size_t size, allc_u8_t alignment)
{
    // Align the current used offset
    size_t aligned_offset = ALIGN_UP(self->buf.used, alignment);
    if (aligned_offset + size > self->buf.size) {
        // Not enough space left in the allocator
        return NULL;
    }

    void *ptr = self->buf.memory + aligned_offset;
    self->buf.used = aligned_offset + size;
    return ptr;
}

// Bump Allocator {{{2
// -------------------

// Bump Allocator - 8bit {{{3

allc_u8_t allc_allocator_bump_alloc_fn8(allc_allocator_bump_s *self, allc_u8_t sz)
{
    return allc_allocator_bump_alloc(self, sz, self->alignment);
}

// Bump Allocator - 16bit {{{3

allc_u16_t allc_allocator_bump_alloc_fn16(allc_allocator_bump_s *self, allc_u16_t sz)
{
    return allc_allocator_bump_alloc(self, sz, self->alignment);
}

// Bump Allocator - 32bit {{{3

allc_u32_t allc_allocator_bump_alloc_fn32(allc_allocator_bump_s *self, allc_u32_t sz)
{
    return allc_allocator_bump_alloc(self, sz, self->alignment);
}

// Bump Allocator - 64bit {{{3

allc_u64_t allc_allocator_bump_alloc_fn64(allc_allocator_bump_s *self, allc_u64_t sz)
{
    return allc_allocator_bump_alloc(self, sz, self->alignment);
}

// Bump Allocator Methods {{{3

allc_allocator_bump_s *allc_allocator_bump_new(allc_allocator_kind_e kind, allc_allocator_s *owner, size_t size)
{}
void allc_allocator_bump_init(allc_allocator_bump_s *self, allc_allocator_kind_e kind, size_t size)
{}


void allc_allocator_bump_reset(allc_allocator_bump_s *self)
{
    // Reset the allocator to allow re-use of the memory
    self->used = 0;
}

#endif // ALLC_ALLOCATOR_IMPL
// }}}1

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker

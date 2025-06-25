/**
 * Memory Management Abstraction
 *
 * Description
 * ===========
 *
 * This module provides a flexible allocator abstraction that supports custom
 * memory management strategies. All other allc modules use this abstraction
 * instead of calling malloc/free directly.
 *
 * Features:
 * - Abstract allocator interface supporting custom memory management
 * - System allocator implementation (wraps malloc/free)
 * - Bump allocators for fast allocation/bulk deallocation
 * - Buffer allocators for constrained memory environments
 * - Consistent API across all allocator types
 *
 * Usage Example:
 * ==============
 *
 * ```c
 * #define ALLC_IMPL
 * #include "allocator.h"
 * 
 * int main() {
 *     // Use system allocator (malloc/free wrapper)
 *     allc_allocator_t allocator = allc_allocator_system();
 *     
 *     // Allocate memory
 *     void *ptr = allc_allocator_alloc(allocator, 100);
 *     if (ptr) {
 *         // Use memory...
 *         allc_allocator_free(allocator, ptr);
 *     }
 *     
 *     // Create bump allocator for fast allocation
 *     char buffer[1024];
 *     allc_allocator_t bump = allc_allocator_bump_from_buffer(buffer, sizeof(buffer));
 *     void *fast_ptr = allc_allocator_alloc(bump, 50);
 *     // No need to free individually, whole allocator can be reset
 *     
 *     return 0;
 * }
 * ```
 *
 * Options
 * =======
 *
 * ALLC_IMPL - Include implementation code
 * ALLC_ALLOCATOR_IMPL - Include only allocator implementation
 *
 * Authored by Kaj Munhoz Arfvidsson, 2023.
 * Edited with LLMs.
 */

#ifndef ALLC_ALLOCATOR__GUARD
#define ALLC_ALLOCATOR__GUARD

// Includes {{{1
// =============

#include "types.h"

// Macros {{{1
// ===========

#ifdef ALLC_IMPL
#define ALLC_ALLOCATOR_IMPL
#endif

// Error codes for allocator operations
#define ALLC_ALLOCATOR_SUCCESS        0
#define ALLC_ALLOCATOR_ERROR_NULL     1
#define ALLC_ALLOCATOR_ERROR_NOMEM    2
#define ALLC_ALLOCATOR_ERROR_INVALID  3

// Type Definitions {{{1
// =====================

// Forward declaration for allocator interface
typedef struct allc_allocator_interface_s allc_allocator_interface_t;

// Allocator function pointer types
typedef void* (*allc_allocator_alloc_fn_t)(allc_allocator_interface_t *self, allc_size_t size);
typedef void* (*allc_allocator_realloc_fn_t)(allc_allocator_interface_t *self, void *ptr, allc_size_t size);
typedef void  (*allc_allocator_free_fn_t)(allc_allocator_interface_t *self, void *ptr);
typedef void  (*allc_allocator_reset_fn_t)(allc_allocator_interface_t *self);

// Allocator interface structure
struct allc_allocator_interface_s {
    allc_allocator_alloc_fn_t   alloc;
    allc_allocator_realloc_fn_t realloc;
    allc_allocator_free_fn_t    free;
    allc_allocator_reset_fn_t   reset;    // Optional: some allocators support reset
    void                       *data;     // Allocator-specific data
};

// Main allocator type - this is what other modules use
typedef allc_allocator_interface_t* allc_allocator_t;

// Function Declarations {{{1
// ==========================

/* Allocator Interface Functions */

/* Allocate memory using the given allocator */
void* allc_allocator_alloc(allc_allocator_t allocator, allc_size_t size);

/* Reallocate memory using the given allocator */
void* allc_allocator_realloc(allc_allocator_t allocator, void *ptr, allc_size_t size);

/* Free memory using the given allocator */
void allc_allocator_free(allc_allocator_t allocator, void *ptr);

/* Reset allocator (if supported) - frees all allocated memory */
void allc_allocator_reset(allc_allocator_t allocator);

/* Check if allocator supports reset operation */
allc_bool_t allc_allocator_can_reset(allc_allocator_t allocator);

/* Allocator Creation Functions */

/* Get the system allocator (malloc/free wrapper) - alias for consistency */
allc_allocator_t allc_allocator_system(void);

/* Convenience alias for system allocator - used in examples */
allc_allocator_t allc_allocator_c(void);

/* Fast allocation macros for performance-critical code */
#define ALLC_ALLOC(allocator, type) \
    ((type*)allc_allocator_alloc((allocator), sizeof(type)))

#define ALLC_ALLOC_ARRAY(allocator, type, count) \
    ((type*)allc_allocator_alloc((allocator), sizeof(type) * (count)))

#define ALLC_FREE(allocator, ptr) \
    allc_allocator_free((allocator), (ptr))

/* Create a bump allocator that uses another allocator for its buffer */
allc_allocator_t allc_allocator_bump_new(allc_allocator_t parent, allc_size_t size);

/* Create a bump allocator from an existing buffer */
allc_allocator_t allc_allocator_bump_from_buffer(void *buffer, allc_size_t size);

/* Create buffer allocators for different address space sizes */
allc_allocator_t allc_allocator_buffer_8bit(allc_u8_t *buffer, allc_u8_t size);
allc_allocator_t allc_allocator_buffer_16bit(allc_u16_t *buffer, allc_u16_t size);
allc_allocator_t allc_allocator_buffer_32bit(allc_u32_t *buffer, allc_u32_t size);
allc_allocator_t allc_allocator_buffer_64bit(allc_u64_t *buffer, allc_u64_t size);

/* Get allocator statistics (if supported) */
typedef struct {
    allc_size_t total_size;
    allc_size_t used_size;
    allc_size_t free_size;
    allc_size_t allocations;
    allc_bool_t can_reset;
} allc_allocator_stats_t;

allc_allocator_stats_t allc_allocator_get_stats(allc_allocator_t allocator);

/* Delete a created allocator (does not affect allocated memory) */
void allc_allocator_delete(allc_allocator_t allocator);

/* Specific Allocator Types */

// System allocator (singleton)
typedef struct {
    allc_allocator_interface_t interface;
} allc_allocator_system_t;

// Bump allocator
typedef struct {
    allc_allocator_interface_t interface;
    allc_u8_t *buffer;
    allc_size_t size;
    allc_size_t used;
    allc_size_t allocations;
    allc_bool_t owns_buffer;
} allc_allocator_bump_t;

// Buffer allocators for different address spaces
typedef struct {
    allc_allocator_interface_t interface;
    allc_u8_t *buffer;
    allc_u8_t size;
    allc_u8_t used;
    allc_u8_t allocations;
} allc_allocator_buffer_8bit_t;

typedef struct {
    allc_allocator_interface_t interface;
    allc_u16_t *buffer;
    allc_u16_t size;
    allc_u16_t used;
    allc_u16_t allocations;
} allc_allocator_buffer_16bit_t;

typedef struct {
    allc_allocator_interface_t interface;
    allc_u32_t *buffer;
    allc_u32_t size;
    allc_u32_t used;
    allc_u32_t allocations;
} allc_allocator_buffer_32bit_t;

typedef struct {
    allc_allocator_interface_t interface;
    allc_u64_t *buffer;
    allc_u64_t size;
    allc_u64_t used;
    allc_u64_t allocations;
} allc_allocator_buffer_64bit_t;

#endif // ALLC_ALLOCATOR__GUARD

/****************************************************************************/

// Implementation {{{1
// ===================

#ifdef ALLC_ALLOCATOR_IMPL
#ifndef ALLC_ALLOCATOR_IMPL_GUARD
#define ALLC_ALLOCATOR_IMPL_GUARD

#include <stdlib.h>
#include <string.h>

// Alignment utility
#define ALLC_ALIGN_UP(size, align) (((size) + (align) - 1) & ~((align) - 1))
#define ALLC_DEFAULT_ALIGNMENT (sizeof(void*))

// Global system allocator instance
static allc_allocator_system_t g_system_allocator;
static allc_bool_t g_system_allocator_initialized = false;

// Interface Functions {{{2
// ========================

void* allc_allocator_alloc(allc_allocator_t allocator, allc_size_t size) {
    if (!allocator || !allocator->alloc) {
        return NULL;
    }
    return allocator->alloc(allocator, size);
}

void* allc_allocator_realloc(allc_allocator_t allocator, void *ptr, allc_size_t size) {
    if (!allocator) {
        return NULL;
    }
    if (allocator->realloc) {
        return allocator->realloc(allocator, ptr, size);
    }
    
    // Fallback: alloc new + copy + free old
    if (size == 0) {
        if (allocator->free) {
            allocator->free(allocator, ptr);
        }
        return NULL;
    }
    
    void *new_ptr = allc_allocator_alloc(allocator, size);
    if (!new_ptr) {
        return NULL;
    }
    
    if (ptr && allocator->free) {
        // We can't know the old size, so this is a basic implementation
        // Real allocators should implement realloc properly
        memcpy(new_ptr, ptr, size); // Assume new size for now
        allocator->free(allocator, ptr);
    }
    
    return new_ptr;
}

void allc_allocator_free(allc_allocator_t allocator, void *ptr) {
    if (!allocator || !ptr) {
        return;
    }
    if (allocator->free) {
        allocator->free(allocator, ptr);
    }
}

void allc_allocator_reset(allc_allocator_t allocator) {
    if (!allocator) {
        return;
    }
    if (allocator->reset) {
        allocator->reset(allocator);
    }
}

allc_bool_t allc_allocator_can_reset(allc_allocator_t allocator) {
    return allocator && allocator->reset != NULL;
}

// System Allocator {{{2
// =====================

static void* allc_allocator_system_alloc(allc_allocator_interface_t *self, allc_size_t size) {
    (void)self; // Unused
    return malloc(size);
}

static void* allc_allocator_system_realloc(allc_allocator_interface_t *self, void *ptr, allc_size_t size) {
    (void)self; // Unused
    return realloc(ptr, size);
}

static void allc_allocator_system_free(allc_allocator_interface_t *self, void *ptr) {
    (void)self; // Unused
    free(ptr);
}

allc_allocator_t allc_allocator_system(void) {
    if (!g_system_allocator_initialized) {
        g_system_allocator.interface.alloc = allc_allocator_system_alloc;
        g_system_allocator.interface.realloc = allc_allocator_system_realloc;
        g_system_allocator.interface.free = allc_allocator_system_free;
        g_system_allocator.interface.reset = NULL; // System allocator doesn't support reset
        g_system_allocator.interface.data = NULL;
        g_system_allocator_initialized = true;
    }
    return &g_system_allocator.interface;
}

allc_allocator_t allc_allocator_c(void) {
    return allc_allocator_system();
}

// Bump Allocator {{{2
// ===================

static void* allc_allocator_bump_alloc(allc_allocator_interface_t *self, allc_size_t size) {
    allc_allocator_bump_t *bump = (allc_allocator_bump_t*)self->data;
    if (!bump) {
        return NULL;
    }
    
    // Align the allocation
    allc_size_t aligned_used = ALLC_ALIGN_UP(bump->used, ALLC_DEFAULT_ALIGNMENT);
    
    if (aligned_used + size > bump->size) {
        return NULL; // Out of memory
    }
    
    void *ptr = bump->buffer + aligned_used;
    bump->used = aligned_used + size;
    bump->allocations++;
    return ptr;
}

static void allc_allocator_bump_free(allc_allocator_interface_t *self, void *ptr) {
    (void)self; // Unused
    (void)ptr;  // Bump allocators don't support individual free
}

static void allc_allocator_bump_reset(allc_allocator_interface_t *self) {
    allc_allocator_bump_t *bump = (allc_allocator_bump_t*)self->data;
    if (bump) {
        bump->used = 0;
        bump->allocations = 0;
    }
}

allc_allocator_t allc_allocator_bump_new(allc_allocator_t parent, allc_size_t size) {
    if (!parent) {
        return NULL;
    }
    
    // Allocate space for both the allocator struct and the buffer
    allc_size_t total_size = sizeof(allc_allocator_bump_t) + size;
    allc_allocator_bump_t *bump = (allc_allocator_bump_t*)allc_allocator_alloc(parent, total_size);
    if (!bump) {
        return NULL;
    }
    
    // Initialize the allocator
    bump->interface.alloc = allc_allocator_bump_alloc;
    bump->interface.realloc = NULL; // Bump allocators typically don't support realloc
    bump->interface.free = allc_allocator_bump_free;
    bump->interface.reset = allc_allocator_bump_reset;
    bump->interface.data = bump; // Point to self
    
    bump->buffer = (allc_u8_t*)(bump + 1); // Buffer starts after the struct
    bump->size = size;
    bump->used = 0;
    bump->allocations = 0;
    bump->owns_buffer = true;
    
    return &bump->interface;
}

allc_allocator_t allc_allocator_bump_from_buffer(void *buffer, allc_size_t size) {
    if (!buffer || size < sizeof(allc_allocator_bump_t)) {
        return NULL;
    }
    
    allc_allocator_bump_t *bump = (allc_allocator_bump_t*)buffer;
    
    // Initialize the allocator
    bump->interface.alloc = allc_allocator_bump_alloc;
    bump->interface.realloc = NULL;
    bump->interface.free = allc_allocator_bump_free;
    bump->interface.reset = allc_allocator_bump_reset;
    bump->interface.data = bump;
    
    bump->buffer = (allc_u8_t*)(bump + 1);
    bump->size = size - sizeof(allc_allocator_bump_t);
    bump->used = 0;
    bump->allocations = 0;
    bump->owns_buffer = false;
    
    return &bump->interface;
}

// Buffer Allocators for Different Address Spaces {{{2
// ===================================================

// 8-bit buffer allocator
static void* allc_allocator_buffer_8bit_alloc(allc_allocator_interface_t *self, allc_size_t size) {
    allc_allocator_buffer_8bit_t *buf = (allc_allocator_buffer_8bit_t*)self->data;
    if (!buf || size > UINT8_MAX) {
        return NULL;
    }
    
    allc_u8_t aligned_used = (allc_u8_t)ALLC_ALIGN_UP(buf->used, sizeof(void*));
    if (aligned_used + size > buf->size) {
        return NULL;
    }
    
    void *ptr = buf->buffer + aligned_used;
    buf->used = aligned_used + (allc_u8_t)size;
    buf->allocations++;
    return ptr;
}

static void allc_allocator_buffer_8bit_free(allc_allocator_interface_t *self, void *ptr) {
    (void)self; (void)ptr; // 8-bit allocators don't support individual free
}

static void allc_allocator_buffer_8bit_reset(allc_allocator_interface_t *self) {
    allc_allocator_buffer_8bit_t *buf = (allc_allocator_buffer_8bit_t*)self->data;
    if (buf) {
        buf->used = 0;
        buf->allocations = 0;
    }
}

allc_allocator_t allc_allocator_buffer_8bit(allc_u8_t *buffer, allc_u8_t size) {
    if (!buffer || size < sizeof(allc_allocator_buffer_8bit_t)) {
        return NULL;
    }
    
    allc_allocator_buffer_8bit_t *buf = (allc_allocator_buffer_8bit_t*)buffer;
    buf->interface.alloc = allc_allocator_buffer_8bit_alloc;
    buf->interface.realloc = NULL;
    buf->interface.free = allc_allocator_buffer_8bit_free;
    buf->interface.reset = allc_allocator_buffer_8bit_reset;
    buf->interface.data = buf;
    
    buf->buffer = buffer + sizeof(allc_allocator_buffer_8bit_t);
    buf->size = size - sizeof(allc_allocator_buffer_8bit_t);
    buf->used = 0;
    buf->allocations = 0;
    
    return &buf->interface;
}

// 16-bit buffer allocator
static void* allc_allocator_buffer_16bit_alloc(allc_allocator_interface_t *self, allc_size_t size) {
    allc_allocator_buffer_16bit_t *buf = (allc_allocator_buffer_16bit_t*)self->data;
    if (!buf || size > UINT16_MAX) {
        return NULL;
    }
    
    allc_u16_t aligned_used = (allc_u16_t)ALLC_ALIGN_UP(buf->used, sizeof(void*));
    if (aligned_used + size > buf->size) {
        return NULL;
    }
    
    void *ptr = (allc_u8_t*)buf->buffer + aligned_used;
    buf->used = aligned_used + (allc_u16_t)size;
    buf->allocations++;
    return ptr;
}

static void allc_allocator_buffer_16bit_free(allc_allocator_interface_t *self, void *ptr) {
    (void)self; (void)ptr;
}

static void allc_allocator_buffer_16bit_reset(allc_allocator_interface_t *self) {
    allc_allocator_buffer_16bit_t *buf = (allc_allocator_buffer_16bit_t*)self->data;
    if (buf) {
        buf->used = 0;
        buf->allocations = 0;
    }
}

allc_allocator_t allc_allocator_buffer_16bit(allc_u16_t *buffer, allc_u16_t size) {
    if (!buffer || size < sizeof(allc_allocator_buffer_16bit_t) / sizeof(allc_u16_t)) {
        return NULL;
    }
    
    allc_allocator_buffer_16bit_t *buf = (allc_allocator_buffer_16bit_t*)buffer;
    buf->interface.alloc = allc_allocator_buffer_16bit_alloc;
    buf->interface.realloc = NULL;
    buf->interface.free = allc_allocator_buffer_16bit_free;
    buf->interface.reset = allc_allocator_buffer_16bit_reset;
    buf->interface.data = buf;
    
    buf->buffer = buffer + (sizeof(allc_allocator_buffer_16bit_t) / sizeof(allc_u16_t));
    buf->size = size - (sizeof(allc_allocator_buffer_16bit_t) / sizeof(allc_u16_t));
    buf->used = 0;
    buf->allocations = 0;
    
    return &buf->interface;
}

// 32-bit buffer allocator
static void* allc_allocator_buffer_32bit_alloc(allc_allocator_interface_t *self, allc_size_t size) {
    allc_allocator_buffer_32bit_t *buf = (allc_allocator_buffer_32bit_t*)self->data;
    if (!buf || size > UINT32_MAX) {
        return NULL;
    }
    
    allc_u32_t aligned_used = (allc_u32_t)ALLC_ALIGN_UP(buf->used, sizeof(void*));
    if (aligned_used + size > buf->size * sizeof(allc_u32_t)) {
        return NULL;
    }
    
    void *ptr = (allc_u8_t*)buf->buffer + aligned_used;
    buf->used = aligned_used + (allc_u32_t)size;
    buf->allocations++;
    return ptr;
}

static void allc_allocator_buffer_32bit_free(allc_allocator_interface_t *self, void *ptr) {
    (void)self; (void)ptr;
}

static void allc_allocator_buffer_32bit_reset(allc_allocator_interface_t *self) {
    allc_allocator_buffer_32bit_t *buf = (allc_allocator_buffer_32bit_t*)self->data;
    if (buf) {
        buf->used = 0;
        buf->allocations = 0;
    }
}

allc_allocator_t allc_allocator_buffer_32bit(allc_u32_t *buffer, allc_u32_t size) {
    if (!buffer || size < sizeof(allc_allocator_buffer_32bit_t) / sizeof(allc_u32_t)) {
        return NULL;
    }
    
    allc_allocator_buffer_32bit_t *buf = (allc_allocator_buffer_32bit_t*)buffer;
    buf->interface.alloc = allc_allocator_buffer_32bit_alloc;
    buf->interface.realloc = NULL;
    buf->interface.free = allc_allocator_buffer_32bit_free;
    buf->interface.reset = allc_allocator_buffer_32bit_reset;
    buf->interface.data = buf;
    
    buf->buffer = buffer + (sizeof(allc_allocator_buffer_32bit_t) / sizeof(allc_u32_t));
    buf->size = size - (sizeof(allc_allocator_buffer_32bit_t) / sizeof(allc_u32_t));
    buf->used = 0;
    buf->allocations = 0;
    
    return &buf->interface;
}

// 64-bit buffer allocator
static void* allc_allocator_buffer_64bit_alloc(allc_allocator_interface_t *self, allc_size_t size) {
    allc_allocator_buffer_64bit_t *buf = (allc_allocator_buffer_64bit_t*)self->data;
    if (!buf || size > UINT64_MAX) {
        return NULL;
    }
    
    allc_u64_t aligned_used = (allc_u64_t)ALLC_ALIGN_UP(buf->used, sizeof(void*));
    if (aligned_used + size > buf->size * sizeof(allc_u64_t)) {
        return NULL;
    }
    
    void *ptr = (allc_u8_t*)buf->buffer + aligned_used;
    buf->used = aligned_used + (allc_u64_t)size;
    buf->allocations++;
    return ptr;
}

static void allc_allocator_buffer_64bit_free(allc_allocator_interface_t *self, void *ptr) {
    (void)self; (void)ptr;
}

static void allc_allocator_buffer_64bit_reset(allc_allocator_interface_t *self) {
    allc_allocator_buffer_64bit_t *buf = (allc_allocator_buffer_64bit_t*)self->data;
    if (buf) {
        buf->used = 0;
        buf->allocations = 0;
    }
}

allc_allocator_t allc_allocator_buffer_64bit(allc_u64_t *buffer, allc_u64_t size) {
    if (!buffer || size < sizeof(allc_allocator_buffer_64bit_t) / sizeof(allc_u64_t)) {
        return NULL;
    }
    
    allc_allocator_buffer_64bit_t *buf = (allc_allocator_buffer_64bit_t*)buffer;
    buf->interface.alloc = allc_allocator_buffer_64bit_alloc;
    buf->interface.realloc = NULL;
    buf->interface.free = allc_allocator_buffer_64bit_free;
    buf->interface.reset = allc_allocator_buffer_64bit_reset;
    buf->interface.data = buf;
    
    buf->buffer = buffer + (sizeof(allc_allocator_buffer_64bit_t) / sizeof(allc_u64_t));
    buf->size = size - (sizeof(allc_allocator_buffer_64bit_t) / sizeof(allc_u64_t));
    buf->used = 0;
    buf->allocations = 0;
    
    return &buf->interface;
}

// Allocator Statistics {{{2
// =========================

allc_allocator_stats_t allc_allocator_get_stats(allc_allocator_t allocator) {
    allc_allocator_stats_t stats = {0};
    
    if (!allocator) {
        return stats;
    }
    
    stats.can_reset = allc_allocator_can_reset(allocator);
    
    // Check if it's a bump allocator
    if (allocator->data && allocator->reset == allc_allocator_bump_reset) {
        allc_allocator_bump_t *bump = (allc_allocator_bump_t*)allocator->data;
        stats.total_size = bump->size;
        stats.used_size = bump->used;
        stats.free_size = bump->size - bump->used;
        stats.allocations = bump->allocations;
    }
    // Check if it's an 8-bit buffer allocator
    else if (allocator->data && allocator->reset == allc_allocator_buffer_8bit_reset) {
        allc_allocator_buffer_8bit_t *buf = (allc_allocator_buffer_8bit_t*)allocator->data;
        stats.total_size = buf->size;
        stats.used_size = buf->used;
        stats.free_size = buf->size - buf->used;
        stats.allocations = buf->allocations;
    }
    // Check if it's a 16-bit buffer allocator
    else if (allocator->data && allocator->reset == allc_allocator_buffer_16bit_reset) {
        allc_allocator_buffer_16bit_t *buf = (allc_allocator_buffer_16bit_t*)allocator->data;
        stats.total_size = buf->size;
        stats.used_size = buf->used;
        stats.free_size = buf->size - buf->used;
        stats.allocations = buf->allocations;
    }
    // Check if it's a 32-bit buffer allocator
    else if (allocator->data && allocator->reset == allc_allocator_buffer_32bit_reset) {
        allc_allocator_buffer_32bit_t *buf = (allc_allocator_buffer_32bit_t*)allocator->data;
        stats.total_size = buf->size * sizeof(allc_u32_t);
        stats.used_size = buf->used;
        stats.free_size = (buf->size * sizeof(allc_u32_t)) - buf->used;
        stats.allocations = buf->allocations;
    }
    // Check if it's a 64-bit buffer allocator
    else if (allocator->data && allocator->reset == allc_allocator_buffer_64bit_reset) {
        allc_allocator_buffer_64bit_t *buf = (allc_allocator_buffer_64bit_t*)allocator->data;
        stats.total_size = buf->size * sizeof(allc_u64_t);
        stats.used_size = buf->used;
        stats.free_size = (buf->size * sizeof(allc_u64_t)) - buf->used;
        stats.allocations = buf->allocations;
    }
    // System allocator doesn't provide detailed stats
    else {
        stats.total_size = 0; // Unknown
        stats.used_size = 0;  // Unknown
        stats.free_size = 0;  // Unknown
        stats.allocations = 0; // Unknown
    }
    
    return stats;
}

/* Delete a created allocator (does not affect allocated memory) */
void allc_allocator_delete(allc_allocator_t allocator) {
    if (!allocator || allocator == allc_allocator_system()) {
        return; // Can't delete system allocator
    }
    
    // For bump allocators created with allc_allocator_bump_new,
    // we need to free them using the system allocator since they own their buffer
    if (allocator->data && allocator->reset == allc_allocator_bump_reset) {
        allc_allocator_bump_t *bump = (allc_allocator_bump_t*)allocator->data;
        if (bump->owns_buffer) {
            // Free the entire allocator (including buffer) using system allocator
            free(bump);
        }
        // If !owns_buffer, the allocator was created from an existing buffer
        // and we don't need to free anything
    }
    // Other allocator types that are created from existing buffers don't need freeing
}
#endif // ALLC_ALLOCATOR_IMPL_GUARD
#endif // ALLC_ALLOCATOR_IMPL

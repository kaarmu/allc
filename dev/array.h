/**
 * Dynamic Arrays
 * ==============
 *
 * Description
 * -----------
 * Growable array implementation with automatic resizing.
 * Type-agnostic storage with stride-based element access.
 * Slice operations for array views.
 * Built on allocator abstraction.
 *
 * Options
 * -------
 * #define ALLC_IMPL before including to get the implementation.
 *
 * (Authored by Kaj Munhoz Arfvidsson, 2023)
 **/

#ifndef ALLC_ARRAY__GUARD
#define ALLC_ARRAY__GUARD

// Includes {{{1
// =============

#include "allocator.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

// Macros {{{1
// ===========

#ifdef ALLC_IMPL
#define ALLC_ARRAY_IMPL
#endif

// Slice {{{1
// ==========

/**
 * A view into contiguous memory with stride-based element access.
 *
 * Slices provide a way to work with array-like data without owning the memory.
 * They are used both as views into arrays and as the foundation for the array
 * implementation itself.
 **/
typedef struct allc_slice_s {
    void *data;
    size_t stride;  // Size of each element in bytes
    size_t length;  // Number of elements
} allc_slice_t;

/* Get pointer to element at index i in slice. */
void *allc_slice_at(const allc_slice_t *slice, ssize_t i);

/* Clear all elements in slice to zero. */
void allc_slice_clear(const allc_slice_t *slice);

/* Copy elements from src slice to dst slice. */
void allc_slice_copy(const allc_slice_t *dst, const allc_slice_t *src);

/* Create a sub-slice from start to end (exclusive). */
allc_slice_t allc_slice_sub(const allc_slice_t *slice, ssize_t start, ssize_t end);

// Array {{{1
// ==========

/**
 * Dynamic array with automatic resizing.
 *
 * Arrays own their memory and can grow as needed. They are built on top of
 * slices and provide a safe, convenient interface for dynamic storage.
 **/
typedef struct allc_array_s {
    allc_slice_t slice;      // Current view of the array
    size_t capacity;         // Total allocated capacity in elements
    allc_allocator_t allocator;
} allc_array_t;

typedef allc_array_t Array;

// Array - Construction and Destruction {{{2
// -----------------------------------------

/* Create a new array with given element size and initial capacity. */
allc_array_t allc_array_new(allc_allocator_t allocator, size_t element_size, size_t capacity);

/* Create a new array by copying another array. */
allc_array_t allc_array_copy(const allc_array_t *self);

/* Delete the array and free its memory. */
void allc_array_delete(allc_array_t *self);

// Array - Capacity Management {{{2
// --------------------------------

/* Grow the array capacity by n elements. */
void allc_array_grow(allc_array_t *self, size_t n);

/* Ensure the array has at least n elements of capacity. */
void allc_array_ensure_capacity(allc_array_t *self, size_t n);

/* Reserve space for n additional elements beyond current length. */
void allc_array_reserve(allc_array_t *self, size_t n);

// Array - Element Access {{{2
// ---------------------------

/* Get pointer to element at index i. */
void *allc_array_at(const allc_array_t *self, ssize_t i);

/* Get the first element. */
void *allc_array_first(const allc_array_t *self);

/* Get the last element. */
void *allc_array_last(const allc_array_t *self);

/* Check if array is empty. */
bool allc_array_is_empty(const allc_array_t *self);

// Array - Modification {{{2
// -------------------------

/* Clear the array (set length to 0, keep capacity). */
void allc_array_clear(allc_array_t *self);

/* Push an element to the end of the array. */
void allc_array_push(allc_array_t *self, const void *elem);

/* Pop the last element from the array and copy it to out. */
bool allc_array_pop(allc_array_t *self, void *out);

/* Insert an element at the given index. */
void allc_array_insert(allc_array_t *self, ssize_t index, const void *elem);

/* Remove the element at the given index. */
void allc_array_remove(allc_array_t *self, ssize_t index);

/* Resize the array to the given length. */
void allc_array_resize(allc_array_t *self, size_t new_length);

// Array - Slice Operations {{{2
// -----------------------------

/* Get a slice view of the entire array. */
allc_slice_t allc_array_as_slice(const allc_array_t *self);

/* Get a slice view of part of the array. */
allc_slice_t allc_array_slice(const allc_array_t *self, ssize_t start, ssize_t end);

#endif // ALLC_ARRAY__GUARD

/****************************************************************************/

#ifndef ALLC_ARRAY_IMPL__GUARD
#define ALLC_ARRAY_IMPL__GUARD
#else
#undef ALLC_ARRAY_IMPL
#endif

#ifdef ALLC_ARRAY_IMPL

// Includes {{{1
// =============

#include <string.h>

// Helper Functions {{{1
// =====================

static size_t allc_array_normalize_index(size_t length, ssize_t i) {
    if (i < 0) {
        ssize_t pos = (ssize_t)length + i;
        return pos < 0 ? SIZE_MAX : (size_t)pos;  // Return SIZE_MAX for out-of-bounds
    }
    return (size_t)i >= length ? length : (size_t)i;
}

// Slice Implementation {{{1
// =========================

void *allc_slice_at(const allc_slice_t *slice, ssize_t i) {
    if (slice == NULL || slice->data == NULL) return NULL;
    
    size_t index = allc_array_normalize_index(slice->length, i);
    if (index >= slice->length) return NULL;
    
    return (unsigned char*)slice->data + index * slice->stride;
}

void allc_slice_clear(const allc_slice_t *slice) {
    if (slice == NULL || slice->data == NULL) return;
    memset(slice->data, 0, slice->length * slice->stride);
}

void allc_slice_copy(const allc_slice_t *dst, const allc_slice_t *src) {
    if (dst == NULL || src == NULL || dst->data == NULL || src->data == NULL) return;
    if (dst->stride != src->stride) return;
    
    size_t copy_length = dst->length < src->length ? dst->length : src->length;
    memcpy(dst->data, src->data, copy_length * dst->stride);
}

allc_slice_t allc_slice_sub(const allc_slice_t *slice, ssize_t start, ssize_t end) {
    if (slice == NULL || slice->data == NULL) {
        return (allc_slice_t){.data = NULL, .stride = 0, .length = 0};
    }
    
    size_t start_idx = allc_array_normalize_index(slice->length, start);
    size_t end_idx = allc_array_normalize_index(slice->length, end);
    
    if (start_idx >= end_idx || start_idx >= slice->length) {
        return (allc_slice_t){.data = NULL, .stride = slice->stride, .length = 0};
    }
    
    return (allc_slice_t){
        .data = (unsigned char*)slice->data + start_idx * slice->stride,
        .stride = slice->stride,
        .length = end_idx - start_idx
    };
}

// Array Implementation {{{1
// =========================

allc_array_t allc_array_new(allc_allocator_t allocator, size_t element_size, size_t capacity) {
    void *data = capacity > 0 ? allc_allocator_alloc(allocator, element_size * capacity) : NULL;
    
    return (allc_array_t){
        .slice = {
            .data = data,
            .stride = element_size,
            .length = 0
        },
        .capacity = capacity,
        .allocator = allocator
    };
}

allc_array_t allc_array_copy(const allc_array_t *self) {
    if (self == NULL) {
        return (allc_array_t){0};
    }
    
    allc_array_t copy = allc_array_new(self->allocator, self->slice.stride, self->slice.length);
    if (self->slice.data != NULL && copy.slice.data != NULL) {
        memcpy(copy.slice.data, self->slice.data, self->slice.length * self->slice.stride);
        copy.slice.length = self->slice.length;
    }
    
    return copy;
}

void allc_array_delete(allc_array_t *self) {
    if (self == NULL) return;
    
    if (self->slice.data != NULL) {
        allc_allocator_free(self->allocator, self->slice.data);
    }
    
    self->slice.data = NULL;
    self->slice.length = 0;
    self->capacity = 0;
}

void allc_array_grow(allc_array_t *self, size_t n) {
    if (self == NULL || n == 0) return;
    
    size_t new_capacity = self->capacity + n;
    void *new_data = allc_allocator_realloc(self->allocator, self->slice.data, 
                                           new_capacity * self->slice.stride);
    
    if (new_data != NULL) {
        self->slice.data = new_data;
        self->capacity = new_capacity;
    }
}

void allc_array_ensure_capacity(allc_array_t *self, size_t n) {
    if (self == NULL) return;
    
    if (self->capacity < n) {
        // Grow by at least the requested amount, but use exponential growth for efficiency
        size_t grow_amount = n - self->capacity;
        if (grow_amount < self->capacity) {
            grow_amount = self->capacity > 0 ? self->capacity : 1;
        }
        allc_array_grow(self, grow_amount);
    }
}

void allc_array_reserve(allc_array_t *self, size_t n) {
    if (self == NULL) return;
    allc_array_ensure_capacity(self, self->slice.length + n);
}

void *allc_array_at(const allc_array_t *self, ssize_t i) {
    if (self == NULL) return NULL;
    return allc_slice_at(&self->slice, i);
}

void *allc_array_first(const allc_array_t *self) {
    if (self == NULL || self->slice.length == 0) return NULL;
    return self->slice.data;
}

void *allc_array_last(const allc_array_t *self) {
    if (self == NULL || self->slice.length == 0) return NULL;
    return allc_array_at(self, -1);
}

bool allc_array_is_empty(const allc_array_t *self) {
    return self == NULL || self->slice.length == 0;
}

void allc_array_clear(allc_array_t *self) {
    if (self == NULL) return;
    self->slice.length = 0;
}

void allc_array_push(allc_array_t *self, const void *elem) {
    if (self == NULL || elem == NULL) return;
    
    allc_array_ensure_capacity(self, self->slice.length + 1);
    
    void *dest = (unsigned char*)self->slice.data + self->slice.length * self->slice.stride;
    memcpy(dest, elem, self->slice.stride);
    self->slice.length++;
}

bool allc_array_pop(allc_array_t *self, void *out) {
    if (self == NULL || self->slice.length == 0) return false;
    
    if (out != NULL) {
        void *src = (unsigned char*)self->slice.data + (self->slice.length - 1) * self->slice.stride;
        memcpy(out, src, self->slice.stride);
    }
    
    self->slice.length--;
    return true;
}

void allc_array_insert(allc_array_t *self, ssize_t index, const void *elem) {
    if (self == NULL || elem == NULL) return;
    
    size_t insert_idx = allc_array_normalize_index(self->slice.length + 1, index);
    if (insert_idx > self->slice.length) {
        insert_idx = self->slice.length; // Append at end
    }
    
    allc_array_ensure_capacity(self, self->slice.length + 1);
    
    // Shift elements to the right
    if (insert_idx < self->slice.length) {
        void *src = (unsigned char*)self->slice.data + insert_idx * self->slice.stride;
        void *dst = (unsigned char*)self->slice.data + (insert_idx + 1) * self->slice.stride;
        size_t move_size = (self->slice.length - insert_idx) * self->slice.stride;
        memmove(dst, src, move_size);
    }
    
    // Insert the new element
    void *dest = (unsigned char*)self->slice.data + insert_idx * self->slice.stride;
    memcpy(dest, elem, self->slice.stride);
    self->slice.length++;
}

void allc_array_remove(allc_array_t *self, ssize_t index) {
    if (self == NULL || self->slice.length == 0) return;
    
    size_t remove_idx = allc_array_normalize_index(self->slice.length, index);
    if (remove_idx >= self->slice.length) return;
    
    // Shift elements to the left
    if (remove_idx < self->slice.length - 1) {
        void *dst = (unsigned char*)self->slice.data + remove_idx * self->slice.stride;
        void *src = (unsigned char*)self->slice.data + (remove_idx + 1) * self->slice.stride;
        size_t move_size = (self->slice.length - remove_idx - 1) * self->slice.stride;
        memmove(dst, src, move_size);
    }
    
    self->slice.length--;
}

void allc_array_resize(allc_array_t *self, size_t new_length) {
    if (self == NULL) return;
    
    allc_array_ensure_capacity(self, new_length);
    
    // If growing, zero out new elements
    if (new_length > self->slice.length) {
        void *start = (unsigned char*)self->slice.data + self->slice.length * self->slice.stride;
        size_t clear_size = (new_length - self->slice.length) * self->slice.stride;
        memset(start, 0, clear_size);
    }
    
    self->slice.length = new_length;
}

allc_slice_t allc_array_as_slice(const allc_array_t *self) {
    if (self == NULL) {
        return (allc_slice_t){.data = NULL, .stride = 0, .length = 0};
    }
    return self->slice;
}

allc_slice_t allc_array_slice(const allc_array_t *self, ssize_t start, ssize_t end) {
    if (self == NULL) {
        return (allc_slice_t){.data = NULL, .stride = 0, .length = 0};
    }
    return allc_slice_sub(&self->slice, start, end);
}

// }}}1

#endif // ALLC_ARRAY_IMPL

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker

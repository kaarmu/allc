#include "../include/allocator.h"

// Allocator Object

Allocator allc_allocator_c() {
    return (Allocator) {
        .alloc = &malloc,
        .realloc = &realloc,
        .free = &free,
    };
}

/**
 * Type System Foundation
 *
 * Description
 * ===========
 *
 * This module provides standardized integer types, platform detection,
 * common type aliases, and type introspection support. It serves as the
 * foundation for all other allc modules.
 *
 * Features:
 * - Standardized integer types (allc_u8_t, allc_i32_t, etc.)
 * - Platform detection and sizing (ALLC_SYSTEM_SIZE)
 * - Common type aliases (allc_string_t, allc_bool_t)
 * - Type introspection support
 * - Size and alignment queries
 * - Type classification functions
 *
 * Usage Example:
 * ==============
 *
 * ```c
 * #define ALLC_IMPL
 * #include "types.h"
 * 
 * int main() {
 *     // Platform detection
 *     printf("System pointer size: %d bits\n", ALLC_SYSTEM_SIZE);
 *     
 *     // Type introspection
 *     allc_u32_t size = allc_types_sizeof(ALLC_TYPE_I32);
 *     allc_string_t name = allc_types_name(ALLC_TYPE_I32);
 *     printf("Type %s is %u bytes\n", name, size);
 *     
 *     // Type classification
 *     if (allc_types_is_integer(ALLC_TYPE_I32)) {
 *         printf("i32 is an integer type\n");
 *     }
 *     
 *     return 0;
 * }
 * ```
 *
 * Options
 * =======
 *
 * ALLC_IMPL - Include implementation code
 * ALLC_TYPES_IMPL - Include only types implementation
 *
 * Authored by Kaj Munhoz Arfvidsson, 2023.
 * Edited with LLMs.
 */

#ifndef ALLC_TYPES__GUARD
#define ALLC_TYPES__GUARD

// Includes {{{1
// =============

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

// Macros {{{1
// ===========

#ifdef ALLC_IMPL
#define ALLC_TYPES_IMPL
#endif

// Error codes for types module
#define ALLC_TYPES_SUCCESS        0
#define ALLC_TYPES_ERROR_INVALID  1
#define ALLC_TYPES_ERROR_UNKNOWN  2

// Type Definitions {{{1
// =====================

// Boolean type
typedef bool allc_bool_t;

// Integer types
typedef int8_t  allc_i8_t;
typedef uint8_t allc_u8_t;
typedef int16_t allc_i16_t;
typedef uint16_t allc_u16_t;
typedef int32_t allc_i32_t;
typedef uint32_t allc_u32_t;
typedef int64_t allc_i64_t;
typedef uint64_t allc_u64_t;

// Floating point types
typedef float  allc_f32_t;
typedef double allc_f64_t;

// Platform Detection {{{1
// =======================

#if     SIZE_MAX == 0xFF
#   define ALLC_SYSTEM_SIZE 8
#elif   SIZE_MAX == 0xFFFF
#   define ALLC_SYSTEM_SIZE 16
#elif   SIZE_MAX == 0xFFFFFFFF
#   define ALLC_SYSTEM_SIZE 32
#elif   SIZE_MAX == 0xFFFFFFFFFFFFFFFF
#   define ALLC_SYSTEM_SIZE 64
#else
#   error "Cannot determine system pointer size"
#endif

// Platform-specific pointer size type
#if     ALLC_SYSTEM_SIZE == 8
typedef allc_u8_t  allc_ptr_size_t;
#elif   ALLC_SYSTEM_SIZE == 16 
typedef allc_u16_t allc_ptr_size_t;
#elif   ALLC_SYSTEM_SIZE == 32 
typedef allc_u32_t allc_ptr_size_t;
#elif   ALLC_SYSTEM_SIZE == 64
typedef allc_u64_t allc_ptr_size_t;
#endif

// Common Type Aliases {{{1
// ========================

typedef void *      allc_ptr_t;
typedef allc_u8_t   allc_char_t;
typedef const char *allc_string_t;
typedef char *      allc_chararr_t;
typedef allc_u32_t  allc_error_t;
typedef size_t      allc_size_t;
typedef ptrdiff_t   allc_ptrdiff_t;

// Type Introspection {{{1
// =======================

typedef enum {
    ALLC_TYPE_NONE = 0,
    ALLC_TYPE_BOOL,
    ALLC_TYPE_I8,
    ALLC_TYPE_U8,
    ALLC_TYPE_I16,
    ALLC_TYPE_U16,
    ALLC_TYPE_I32,
    ALLC_TYPE_U32,
    ALLC_TYPE_I64,
    ALLC_TYPE_U64,
    ALLC_TYPE_F32,
    ALLC_TYPE_F64,
    ALLC_TYPE_CHAR,
    ALLC_TYPE_STRING,
    ALLC_TYPE_CHARARR,
    ALLC_TYPE_PTR,
    ALLC_TYPE_ERROR,
    ALLC_TYPE_SIZE,
    ALLC_TYPE_PTRDIFF,
    ALLC_TYPE_COUNT  // Total number of types - keep last
} allc_type_e;

typedef struct {
    allc_u32_t size;
    allc_u32_t alignment;
    allc_u16_t hash;
    allc_u16_t flags;
    allc_string_t name;
} allc_introspect_t;

// Function Declarations {{{1
// ==========================

/* Get type information for a given type enum */
allc_introspect_t allc_types_get_info(allc_type_e type);

/* Get the size in bytes for a given type */
allc_u32_t allc_types_sizeof(allc_type_e type);

/* Get the name string for a given type */
allc_string_t allc_types_name(allc_type_e type);

/* Check if a type is an integer type */
allc_bool_t allc_types_is_integer(allc_type_e type);

/* Check if a type is a floating point type */
allc_bool_t allc_types_is_float(allc_type_e type);

/* Check if a type is a pointer type */
allc_bool_t allc_types_is_pointer(allc_type_e type);

/* Get the alignment requirement for a given type */
allc_u32_t allc_types_alignof(allc_type_e type);

/* Check if a type enum is valid */
allc_bool_t allc_types_is_valid(allc_type_e type);

/* Get total number of supported types */
allc_u32_t allc_types_count(void);

#endif // ALLC_TYPES__GUARD

/****************************************************************************/

// Implementation {{{1
// ===================

#ifndef ALLC_TYPES_IMPL__GUARD
#define ALLC_TYPES_IMPL__GUARD
#else
#undef ALLC_TYPES_IMPL
#endif

#ifdef ALLC_TYPES_IMPL

// Type information lookup table
static const allc_introspect_t allc_type_info_table[] = {
    [ALLC_TYPE_NONE]     = {0,                     0, 0, 0, "none"},
    [ALLC_TYPE_BOOL]     = {sizeof(allc_bool_t),   _Alignof(allc_bool_t), 0, 0, "bool"},
    [ALLC_TYPE_I8]       = {sizeof(allc_i8_t),     _Alignof(allc_i8_t), 0, 0, "i8"},
    [ALLC_TYPE_U8]       = {sizeof(allc_u8_t),     _Alignof(allc_u8_t), 0, 0, "u8"},
    [ALLC_TYPE_I16]      = {sizeof(allc_i16_t),    _Alignof(allc_i16_t), 0, 0, "i16"},
    [ALLC_TYPE_U16]      = {sizeof(allc_u16_t),    _Alignof(allc_u16_t), 0, 0, "u16"},
    [ALLC_TYPE_I32]      = {sizeof(allc_i32_t),    _Alignof(allc_i32_t), 0, 0, "i32"},
    [ALLC_TYPE_U32]      = {sizeof(allc_u32_t),    _Alignof(allc_u32_t), 0, 0, "u32"},
    [ALLC_TYPE_I64]      = {sizeof(allc_i64_t),    _Alignof(allc_i64_t), 0, 0, "i64"},
    [ALLC_TYPE_U64]      = {sizeof(allc_u64_t),    _Alignof(allc_u64_t), 0, 0, "u64"},
    [ALLC_TYPE_F32]      = {sizeof(allc_f32_t),    _Alignof(allc_f32_t), 0, 0, "f32"},
    [ALLC_TYPE_F64]      = {sizeof(allc_f64_t),    _Alignof(allc_f64_t), 0, 0, "f64"},
    [ALLC_TYPE_CHAR]     = {sizeof(allc_char_t),   _Alignof(allc_char_t), 0, 0, "char"},
    [ALLC_TYPE_STRING]   = {sizeof(allc_string_t), _Alignof(allc_string_t), 0, 0, "string"},
    [ALLC_TYPE_CHARARR]  = {sizeof(allc_chararr_t), _Alignof(allc_chararr_t), 0, 0, "chararr"},
    [ALLC_TYPE_PTR]      = {sizeof(allc_ptr_t),    _Alignof(allc_ptr_t), 0, 0, "ptr"},
    [ALLC_TYPE_ERROR]    = {sizeof(allc_error_t),  _Alignof(allc_error_t), 0, 0, "error"},
    [ALLC_TYPE_SIZE]     = {sizeof(allc_size_t),   _Alignof(allc_size_t), 0, 0, "size"},
    [ALLC_TYPE_PTRDIFF]  = {sizeof(allc_ptrdiff_t), _Alignof(allc_ptrdiff_t), 0, 0, "ptrdiff"},
};

allc_introspect_t allc_types_get_info(allc_type_e type) {
    if (type < 0 || type >= ALLC_TYPE_COUNT) {
        return allc_type_info_table[ALLC_TYPE_NONE];
    }
    return allc_type_info_table[type];
}

allc_u32_t allc_types_sizeof(allc_type_e type) {
    return allc_types_get_info(type).size;
}

allc_string_t allc_types_name(allc_type_e type) {
    return allc_types_get_info(type).name;
}

allc_bool_t allc_types_is_integer(allc_type_e type) {
    switch (type) {
        case ALLC_TYPE_I8:
        case ALLC_TYPE_U8:
        case ALLC_TYPE_I16:
        case ALLC_TYPE_U16:
        case ALLC_TYPE_I32:
        case ALLC_TYPE_U32:
        case ALLC_TYPE_I64:
        case ALLC_TYPE_U64:
        case ALLC_TYPE_CHAR:
        case ALLC_TYPE_SIZE:
        case ALLC_TYPE_PTRDIFF:
            return true;
        default:
            return false;
    }
}

allc_bool_t allc_types_is_float(allc_type_e type) {
    return (type == ALLC_TYPE_F32 || type == ALLC_TYPE_F64);
}

allc_bool_t allc_types_is_pointer(allc_type_e type) {
    switch (type) {
        case ALLC_TYPE_STRING:
        case ALLC_TYPE_CHARARR:
        case ALLC_TYPE_PTR:
            return true;
        default:
            return false;
    }
}

allc_u32_t allc_types_alignof(allc_type_e type) {
    return allc_types_get_info(type).alignment;
}

allc_bool_t allc_types_is_valid(allc_type_e type) {
    return (type >= ALLC_TYPE_NONE && type < ALLC_TYPE_COUNT);
}

allc_u32_t allc_types_count(void) {
    return ALLC_TYPE_COUNT;
}

#endif // ALLC_TYPES_IMPL
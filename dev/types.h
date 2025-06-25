#ifndef ALLC_TYPES__GUARD
#define ALLC_TYPES__GUARD

#include <stdbool.h>
#include <stdint.h>

typedef bool                    allc_bool_t;

typedef int8_t      allc_i8_t;
typedef uint8_t     allc_u8_t;
typedef int16_t     allc_i16_t;
typedef uint16_t    allc_u16_t;
typedef int32_t     allc_i32_t;
typedef uint32_t    allc_u32_t;
typedef int64_t     allc_i64_t;
typedef uint64_t    allc_u64_t;

typedef float                   allc_f32_t;
typedef double                  allc_f64_t;

#if     SIZE_MAX == 0xFF
#   define ALLC_SYSTEM_SIZE 8
#elif   SIZE_MAX == 0xFFFF
#   define ALLC_SYSTEM_SIZE 16
#elif   SIZE_MAX == 0xFFFFFFFF
#   define ALLC_SYSTEM_SIZE 32
#elif   SIZE_MAX == 0xFFFFFFFFFFFFFFFF
#   define ALLC_SYSTEM_SIZE 64
#endif

#if     ALLC_SYSTEM_SIZE == 8
typedef allc_u8_t   allc_ptr_size_t;
#elif   ALLC_SYSTEM_SIZE == 16 
typedef allc_u16_t  allc_ptr_size_t;
#elif   ALLC_SYSTEM_SIZE == 32 
typedef allc_u32_t  allc_ptr_size_t;
#elif   ALLC_SYSTEM_SIZE == 64
typedef allc_u64_t  allc_ptr_size_t;
#endif

typedef void *                  allc_ptr_t;
typedef allc_u8_t               allc_char_t;
typedef const char *            allc_string_t;
typedef char *                  allc_chararr_t;

typedef allc_u32_t              allc_error_t;

typedef enum {
    allc_types_none,
    allc_types_bool,
    allc_types_i8,
    allc_types_u8,
    allc_types_i16,
    allc_types_u16,
    allc_types_i32,
    allc_types_u32,
    allc_types_i64,
    allc_types_u64,
    allc_types_f32,
    allc_types_f64,
    allc_types_char,
    allc_types_string,
    allc_types_chararr,
    allc_types_error,
} allc_type_e;

struct allc_introspect_s {
    allc_u32_t size;
    allc_u16_t hash;
    allc_u16_t fields;
    allc_string_t name;
};


#endif // ALLC_TYPES__GUARD
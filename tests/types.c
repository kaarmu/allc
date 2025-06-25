#define ALLC_IMPL

#include "../dev/types.h"
#include "../dev/macro.h"

#include <stdio.h>
#include <string.h>

void test__allc_type_sizes() {
    printf("\n[%s]\n", __func__);
    
    // Test standard integer sizes
    ALLC_TEST_ANY("1", "%zu", sizeof(allc_i8_t));
    ALLC_TEST_ANY("1", "%zu", sizeof(allc_u8_t));
    ALLC_TEST_ANY("2", "%zu", sizeof(allc_i16_t));
    ALLC_TEST_ANY("2", "%zu", sizeof(allc_u16_t));
    ALLC_TEST_ANY("4", "%zu", sizeof(allc_i32_t));
    ALLC_TEST_ANY("4", "%zu", sizeof(allc_u32_t));
    ALLC_TEST_ANY("8", "%zu", sizeof(allc_i64_t));
    ALLC_TEST_ANY("8", "%zu", sizeof(allc_u64_t));
    
    // Test floating point sizes
    ALLC_TEST_ANY("4", "%zu", sizeof(allc_f32_t));
    ALLC_TEST_ANY("8", "%zu", sizeof(allc_f64_t));
    
    // Test boolean size
    ALLC_TEST_ANY("1", "%zu", sizeof(allc_bool_t));
}

void test__allc_platform_detection() {
    printf("\n[%s]\n", __func__);
    
    // Test that ALLC_SYSTEM_SIZE is defined and has a valid value
    ALLC_TEST_BOOL(true, ALLC_SYSTEM_SIZE == 8 || ALLC_SYSTEM_SIZE == 16 || 
                         ALLC_SYSTEM_SIZE == 32 || ALLC_SYSTEM_SIZE == 64);
    
    // Test that ptr_size_t matches the system size
    ALLC_TEST_BOOL(true, sizeof(allc_ptr_size_t) * 8 == ALLC_SYSTEM_SIZE);
    
    // Test that pointer size matches system expectations
    ALLC_TEST_BOOL(true, sizeof(void*) == sizeof(allc_ptr_size_t));
    
    printf("System size: %d bits\n", ALLC_SYSTEM_SIZE);
    printf("Pointer size: %zu bytes\n", sizeof(allc_ptr_t));
    printf("allc_ptr_size_t size: %zu bytes\n", sizeof(allc_ptr_size_t));
}

void test__allc_type_aliases() {
    printf("\n[%s]\n", __func__);
    
    // Test common type aliases exist and have expected sizes
    ALLC_TEST_ANY("1", "%zu", sizeof(allc_char_t));
    ALLC_TEST_BOOL(true, sizeof(allc_string_t) == sizeof(const char*));
    ALLC_TEST_BOOL(true, sizeof(allc_chararr_t) == sizeof(char*));
    ALLC_TEST_BOOL(true, sizeof(allc_ptr_t) == sizeof(void*));
    ALLC_TEST_ANY("4", "%zu", sizeof(allc_error_t));
    
    // Test new type aliases
    ALLC_TEST_BOOL(true, sizeof(allc_size_t) == sizeof(size_t));
    ALLC_TEST_BOOL(true, sizeof(allc_ptrdiff_t) == sizeof(ptrdiff_t));
}

void test__allc_types_get_info() {
    printf("\n[%s]\n", __func__);
    
    // Test basic type info retrieval
    allc_introspect_t info;
    
    info = allc_types_get_info(ALLC_TYPE_I32);
    ALLC_TEST_ANY("4", "%u", info.size);
    ALLC_TEST_ANY("i32", "%s", info.name);
    ALLC_TEST_BOOL(true, info.alignment > 0);
    
    info = allc_types_get_info(ALLC_TYPE_BOOL);
    ALLC_TEST_ANY("1", "%u", info.size);
    ALLC_TEST_ANY("bool", "%s", info.name);
    
    info = allc_types_get_info(ALLC_TYPE_F64);
    ALLC_TEST_ANY("8", "%u", info.size);
    ALLC_TEST_ANY("f64", "%s", info.name);
    
    // Test invalid type returns NONE
    info = allc_types_get_info((allc_type_e)999);
    ALLC_TEST_ANY("0", "%u", info.size);
    ALLC_TEST_ANY("none", "%s", info.name);
}

void test__allc_types_sizeof() {
    printf("\n[%s]\n", __func__);
    
    // Test sizeof function for various types
    ALLC_TEST_ANY("1", "%u", allc_types_sizeof(ALLC_TYPE_I8));
    ALLC_TEST_ANY("1", "%u", allc_types_sizeof(ALLC_TYPE_U8));
    ALLC_TEST_ANY("2", "%u", allc_types_sizeof(ALLC_TYPE_I16));
    ALLC_TEST_ANY("2", "%u", allc_types_sizeof(ALLC_TYPE_U16));
    ALLC_TEST_ANY("4", "%u", allc_types_sizeof(ALLC_TYPE_I32));
    ALLC_TEST_ANY("4", "%u", allc_types_sizeof(ALLC_TYPE_U32));
    ALLC_TEST_ANY("8", "%u", allc_types_sizeof(ALLC_TYPE_I64));
    ALLC_TEST_ANY("8", "%u", allc_types_sizeof(ALLC_TYPE_U64));
    ALLC_TEST_ANY("4", "%u", allc_types_sizeof(ALLC_TYPE_F32));
    ALLC_TEST_ANY("8", "%u", allc_types_sizeof(ALLC_TYPE_F64));
    
    // Test pointer types
    ALLC_TEST_BOOL(true, allc_types_sizeof(ALLC_TYPE_STRING) == sizeof(const char*));
    ALLC_TEST_BOOL(true, allc_types_sizeof(ALLC_TYPE_PTR) == sizeof(void*));
    
    // Test new types
    ALLC_TEST_BOOL(true, allc_types_sizeof(ALLC_TYPE_SIZE) == sizeof(size_t));
    ALLC_TEST_BOOL(true, allc_types_sizeof(ALLC_TYPE_PTRDIFF) == sizeof(ptrdiff_t));
}

void test__allc_types_name() {
    printf("\n[%s]\n", __func__);
    
    // Test name function for various types
    ALLC_TEST_ANY("i8", "%s", allc_types_name(ALLC_TYPE_I8));
    ALLC_TEST_ANY("u8", "%s", allc_types_name(ALLC_TYPE_U8));
    ALLC_TEST_ANY("i16", "%s", allc_types_name(ALLC_TYPE_I16));
    ALLC_TEST_ANY("u16", "%s", allc_types_name(ALLC_TYPE_U16));
    ALLC_TEST_ANY("i32", "%s", allc_types_name(ALLC_TYPE_I32));
    ALLC_TEST_ANY("u32", "%s", allc_types_name(ALLC_TYPE_U32));
    ALLC_TEST_ANY("i64", "%s", allc_types_name(ALLC_TYPE_I64));
    ALLC_TEST_ANY("u64", "%s", allc_types_name(ALLC_TYPE_U64));
    ALLC_TEST_ANY("f32", "%s", allc_types_name(ALLC_TYPE_F32));
    ALLC_TEST_ANY("f64", "%s", allc_types_name(ALLC_TYPE_F64));
    ALLC_TEST_ANY("bool", "%s", allc_types_name(ALLC_TYPE_BOOL));
    ALLC_TEST_ANY("char", "%s", allc_types_name(ALLC_TYPE_CHAR));
    ALLC_TEST_ANY("string", "%s", allc_types_name(ALLC_TYPE_STRING));
    ALLC_TEST_ANY("chararr", "%s", allc_types_name(ALLC_TYPE_CHARARR));
    ALLC_TEST_ANY("ptr", "%s", allc_types_name(ALLC_TYPE_PTR));
    ALLC_TEST_ANY("error", "%s", allc_types_name(ALLC_TYPE_ERROR));
    ALLC_TEST_ANY("size", "%s", allc_types_name(ALLC_TYPE_SIZE));
    ALLC_TEST_ANY("ptrdiff", "%s", allc_types_name(ALLC_TYPE_PTRDIFF));
}

void test__allc_types_is_integer() {
    printf("\n[%s]\n", __func__);
    
    // Test integer type detection
    ALLC_TEST_BOOL(true, allc_types_is_integer(ALLC_TYPE_I8));
    ALLC_TEST_BOOL(true, allc_types_is_integer(ALLC_TYPE_U8));
    ALLC_TEST_BOOL(true, allc_types_is_integer(ALLC_TYPE_I16));
    ALLC_TEST_BOOL(true, allc_types_is_integer(ALLC_TYPE_U16));
    ALLC_TEST_BOOL(true, allc_types_is_integer(ALLC_TYPE_I32));
    ALLC_TEST_BOOL(true, allc_types_is_integer(ALLC_TYPE_U32));
    ALLC_TEST_BOOL(true, allc_types_is_integer(ALLC_TYPE_I64));
    ALLC_TEST_BOOL(true, allc_types_is_integer(ALLC_TYPE_U64));
    ALLC_TEST_BOOL(true, allc_types_is_integer(ALLC_TYPE_CHAR));
    ALLC_TEST_BOOL(true, allc_types_is_integer(ALLC_TYPE_SIZE));
    ALLC_TEST_BOOL(true, allc_types_is_integer(ALLC_TYPE_PTRDIFF));
    
    // Test non-integer types
    ALLC_TEST_BOOL(false, allc_types_is_integer(ALLC_TYPE_F32));
    ALLC_TEST_BOOL(false, allc_types_is_integer(ALLC_TYPE_F64));
    ALLC_TEST_BOOL(false, allc_types_is_integer(ALLC_TYPE_BOOL));
    ALLC_TEST_BOOL(false, allc_types_is_integer(ALLC_TYPE_STRING));
    ALLC_TEST_BOOL(false, allc_types_is_integer(ALLC_TYPE_PTR));
}

void test__allc_types_is_float() {
    printf("\n[%s]\n", __func__);
    
    // Test float type detection
    ALLC_TEST_BOOL(true, allc_types_is_float(ALLC_TYPE_F32));
    ALLC_TEST_BOOL(true, allc_types_is_float(ALLC_TYPE_F64));
    
    // Test non-float types
    ALLC_TEST_BOOL(false, allc_types_is_float(ALLC_TYPE_I32));
    ALLC_TEST_BOOL(false, allc_types_is_float(ALLC_TYPE_U32));
    ALLC_TEST_BOOL(false, allc_types_is_float(ALLC_TYPE_BOOL));
    ALLC_TEST_BOOL(false, allc_types_is_float(ALLC_TYPE_STRING));
}

void test__allc_types_is_pointer() {
    printf("\n[%s]\n", __func__);
    
    // Test pointer type detection
    ALLC_TEST_BOOL(true, allc_types_is_pointer(ALLC_TYPE_STRING));
    ALLC_TEST_BOOL(true, allc_types_is_pointer(ALLC_TYPE_CHARARR));
    ALLC_TEST_BOOL(true, allc_types_is_pointer(ALLC_TYPE_PTR));
    
    // Test non-pointer types
    ALLC_TEST_BOOL(false, allc_types_is_pointer(ALLC_TYPE_I32));
    ALLC_TEST_BOOL(false, allc_types_is_pointer(ALLC_TYPE_F32));
    ALLC_TEST_BOOL(false, allc_types_is_pointer(ALLC_TYPE_BOOL));
    ALLC_TEST_BOOL(false, allc_types_is_pointer(ALLC_TYPE_CHAR));
}

void test__allc_type_enum_consistency() {
    printf("\n[%s]\n", __func__);
    
    // Test that all enum values have corresponding entries
    for (int i = 0; i < ALLC_TYPE_COUNT; i++) {
        allc_introspect_t info = allc_types_get_info((allc_type_e)i);
        ALLC_TEST_BOOL(true, info.name != NULL);
        ALLC_TEST_BOOL(true, info.size > 0 || i == ALLC_TYPE_NONE);
        printf("Type %d: %s (size: %u, alignment: %u)\n", i, info.name, info.size, info.alignment);
    }
    
    // Test type count function
    ALLC_TEST_ANY("equal", "%s", (allc_types_count() == ALLC_TYPE_COUNT) ? "equal" : "different");
}

void test__allc_types_new_functions() {
    printf("\n[%s]\n", __func__);
    
    // Test alignment function
    ALLC_TEST_BOOL(true, allc_types_alignof(ALLC_TYPE_I32) > 0);
    ALLC_TEST_BOOL(true, allc_types_alignof(ALLC_TYPE_F64) > 0);
    ALLC_TEST_BOOL(true, allc_types_alignof(ALLC_TYPE_PTR) > 0);
    
    // Test validation function
    ALLC_TEST_BOOL(true, allc_types_is_valid(ALLC_TYPE_NONE));
    ALLC_TEST_BOOL(true, allc_types_is_valid(ALLC_TYPE_I32));
    ALLC_TEST_BOOL(true, allc_types_is_valid(ALLC_TYPE_PTRDIFF));
    ALLC_TEST_BOOL(false, allc_types_is_valid(-1));
    ALLC_TEST_BOOL(false, allc_types_is_valid(ALLC_TYPE_COUNT));
    ALLC_TEST_BOOL(false, allc_types_is_valid(999));
    
    // Test error handling
    ALLC_TEST_ANY("0", "%u", allc_types_sizeof(-1));
    ALLC_TEST_ANY("none", "%s", allc_types_name(-1));
    ALLC_TEST_ANY("0", "%u", allc_types_alignof(-1));
}

int main() {
    printf("=== Testing types.h ===\n");
    
    test__allc_type_sizes();
    test__allc_platform_detection();
    test__allc_type_aliases();
    test__allc_types_get_info();
    test__allc_types_sizeof();
    test__allc_types_name();
    test__allc_types_is_integer();
    test__allc_types_is_float();
    test__allc_types_is_pointer();
    test__allc_type_enum_consistency();
    test__allc_types_new_functions();
    
    printf("\n=== types.h tests completed ===\n");
    return 0;
}

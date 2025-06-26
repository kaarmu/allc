/**
 * Tests for array.h
 * =================
 *
 * Comprehensive tests for dynamic arrays and slices.
 */

#define ALLC_IMPL
#include "../dev/array.h"
#include "../dev/macro.h"
#include "../dev/cstr.h"
#include <stdio.h>
#include <string.h>

void test__allc__allc_slice_basic() {
    printf("\n[%s]\n", __func__);
    
    // Test normal cases
    int data[] = {1, 2, 3, 4, 5};
    allc_slice_t slice = {
        .data = data,
        .stride = sizeof(int),
        .length = 5
    };
    
    int *first = (int*)allc_slice_at(&slice, 0);
    int *last = (int*)allc_slice_at(&slice, -1);
    int *middle = (int*)allc_slice_at(&slice, 2);
    
    ALLC_TEST_ANY("1", "%d", first ? *first : -1);
    ALLC_TEST_ANY("5", "%d", last ? *last : -1);
    ALLC_TEST_ANY("3", "%d", middle ? *middle : -1);
    
    // Test error conditions
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_slice_at(&slice, 10) == NULL));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_slice_at(&slice, -10) == NULL));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_slice_at(NULL, 0) == NULL));
}

void test__allc__allc_slice_clear() {
    printf("\n[%s]\n", __func__);
    
    // Test normal case
    int data[] = {1, 2, 3, 4, 5};
    allc_slice_t slice = {
        .data = data,
        .stride = sizeof(int),
        .length = 5
    };
    
    allc_slice_clear(&slice);
    
    ALLC_TEST_ANY("0", "%d", data[0]);
    ALLC_TEST_ANY("0", "%d", data[2]);
    ALLC_TEST_ANY("0", "%d", data[4]);
    
    // Test error conditions
    allc_slice_clear(NULL);  // Should not crash
    slice.data = NULL;
    allc_slice_clear(&slice);  // Should not crash
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(true)); // If we get here, no crash occurred
}

void test__allc__allc_slice_copy() {
    printf("\n[%s]\n", __func__);
    
    // Test normal case
    int src_data[] = {1, 2, 3, 4, 5};
    int dst_data[] = {0, 0, 0, 0, 0};
    
    allc_slice_t src = {
        .data = src_data,
        .stride = sizeof(int),
        .length = 5
    };
    
    allc_slice_t dst = {
        .data = dst_data,
        .stride = sizeof(int),
        .length = 5
    };
    
    allc_slice_copy(&dst, &src);
    
    ALLC_TEST_ANY("1", "%d", dst_data[0]);
    ALLC_TEST_ANY("3", "%d", dst_data[2]);
    ALLC_TEST_ANY("5", "%d", dst_data[4]);
    
    // Test partial copy (dst smaller)
    int small_dst[] = {0, 0, 0};
    allc_slice_t small = {
        .data = small_dst,
        .stride = sizeof(int),
        .length = 3
    };
    
    allc_slice_copy(&small, &src);
    ALLC_TEST_ANY("1", "%d", small_dst[0]);
    ALLC_TEST_ANY("2", "%d", small_dst[1]);
    ALLC_TEST_ANY("3", "%d", small_dst[2]);
    
    // Test error conditions
    allc_slice_copy(NULL, &src);  // Should not crash
    allc_slice_copy(&dst, NULL);  // Should not crash
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(true)); // If we get here, no crash occurred
}

void test__allc__allc_slice_sub() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test normal sub-slice:\n");
    int data[] = {1, 2, 3, 4, 5};
    allc_slice_t slice = {
        .data = data,
        .stride = sizeof(int),
        .length = 5
    };
    
    // Test normal sub-slice
    allc_slice_t sub1 = allc_slice_sub(&slice, 1, 4);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(sub1.data != NULL));
    ALLC_TEST_ANY("3", "%zu", sub1.length);
    ALLC_TEST_ANY("4", "%zu", sub1.stride);
    ALLC_TEST_ANY("2", "%d", *(int*)allc_slice_at(&sub1, 0));
    ALLC_TEST_ANY("4", "%d", *(int*)allc_slice_at(&sub1, -1));
    
    printf(">> Test negative indices:\n");
    allc_slice_t sub2 = allc_slice_sub(&slice, -3, -1);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(sub2.data != NULL));
    ALLC_TEST_ANY("2", "%zu", sub2.length);
    ALLC_TEST_ANY("3", "%d", *(int*)allc_slice_at(&sub2, 0));
    ALLC_TEST_ANY("4", "%d", *(int*)allc_slice_at(&sub2, 1));
    
    printf(">> Test edge cases:\n");
    // Test empty slice
    allc_slice_t empty = allc_slice_sub(&slice, 2, 2);
    ALLC_TEST_ANY("0", "%zu", empty.length);
    
    // Test invalid ranges
    allc_slice_t invalid = allc_slice_sub(&slice, 10, 15);
    ALLC_TEST_ANY("0", "%zu", invalid.length);
    
    printf(">> Test error conditions:\n");
    // Test null safety
    allc_slice_t null_slice = allc_slice_sub(NULL, 0, 1);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(null_slice.data == NULL));
    ALLC_TEST_ANY("0", "%zu", null_slice.length);
}

// Array Tests
// ===========

void test__allc__allc_array_creation() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test basic creation:\n");
    allc_allocator_t alloc = allc_allocator_system();
    
    // Test basic creation
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 10);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(arr.slice.data != NULL));
    ALLC_TEST_ANY("4", "%zu", arr.slice.stride);
    ALLC_TEST_ANY("0", "%zu", arr.slice.length);
    ALLC_TEST_ANY("10", "%zu", arr.capacity);
    
    printf(">> Test cleanup:\n");
    allc_array_delete(&arr);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(arr.slice.data == NULL));
    ALLC_TEST_ANY("0", "%zu", arr.capacity);
    
    printf(">> Test zero capacity:\n");
    allc_array_t zero_cap = allc_array_new(alloc, sizeof(int), 0);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(zero_cap.slice.data == NULL));
    ALLC_TEST_ANY("0", "%zu", zero_cap.capacity);
    allc_array_delete(&zero_cap);
}

void test__allc__allc_array_copy() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test array copy:\n");
    allc_allocator_t alloc = allc_allocator_system();
    
    allc_array_t original = allc_array_new(alloc, sizeof(int), 5);
    
    // Add some data
    for (int i = 1; i <= 5; i++) {
        allc_array_push(&original, &i);
    }
    
    // Test copy
    allc_array_t copy = allc_array_copy(&original);
    ALLC_TEST_ANY("5", "%zu", copy.slice.length);
    ALLC_TEST_ANY("4", "%zu", copy.slice.stride);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(copy.slice.data != original.slice.data));
    
    for (size_t i = 0; i < copy.slice.length; i++) {
        int *orig_val = (int*)allc_array_at(&original, i);
        int *copy_val = (int*)allc_array_at(&copy, i);
        ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(*orig_val == *copy_val));
    }
    
    printf(">> Test error conditions:\n");
    allc_array_t null_copy = allc_array_copy(NULL);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(null_copy.slice.data == NULL));
    ALLC_TEST_ANY("0", "%zu", null_copy.slice.length);
    
    allc_array_delete(&original);
    allc_array_delete(&copy);
}

void test__allc__allc_array_capacity() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test capacity management:\n");
    allc_allocator_t alloc = allc_allocator_system();
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 5);
    
    // Test grow
    allc_array_grow(&arr, 10);
    ALLC_TEST_ANY("15", "%zu", arr.capacity);
    
    // Test ensure_capacity (no growth needed)
    allc_array_ensure_capacity(&arr, 10);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(arr.capacity >= 15));
    
    // Test ensure_capacity (growth needed)
    size_t current_capacity = arr.capacity;
    allc_array_ensure_capacity(&arr, current_capacity + 20);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(arr.capacity >= current_capacity + 20));
    
    // Test reserve
    current_capacity = arr.capacity;
    arr.slice.length = 5;
    allc_array_reserve(&arr, 10);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(arr.capacity >= current_capacity || arr.capacity >= 15));
    
    printf(">> Test error conditions:\n");
    // Test null safety - these should not crash
    allc_array_grow(NULL, 10);
    allc_array_ensure_capacity(NULL, 10);
    allc_array_reserve(NULL, 10);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(true)); // If we get here, no crash occurred
    
    allc_array_delete(&arr);
}

void test__allc__allc_array_access() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test array access:\n");
    allc_allocator_t alloc = allc_allocator_system();
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 5);
    
    // Add test data
    for (int i = 1; i <= 5; i++) {
        allc_array_push(&arr, &i);
    }
    
    // Test basic access
    int *first = (int*)allc_array_first(&arr);
    int *last = (int*)allc_array_last(&arr);
    int *at_index = (int*)allc_array_at(&arr, 2);
    
    ALLC_TEST_ANY("1", "%d", first ? *first : -1);
    ALLC_TEST_ANY("5", "%d", last ? *last : -1);
    ALLC_TEST_ANY("3", "%d", at_index ? *at_index : -1);
    
    printf(">> Test negative indexing:\n");
    int *from_end = (int*)allc_array_at(&arr, -2);
    ALLC_TEST_ANY("4", "%d", from_end ? *from_end : -1);
    
    printf(">> Test bounds checking:\n");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_array_at(&arr, 10) == NULL));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_array_at(&arr, -10) == NULL));
    
    printf(">> Test empty array:\n");
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_array_is_empty(&arr)));
    allc_array_clear(&arr);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_array_is_empty(&arr)));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_array_first(&arr) == NULL));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_array_last(&arr) == NULL));
    
    printf(">> Test error conditions:\n");
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_array_at(NULL, 0) == NULL));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_array_first(NULL) == NULL));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_array_last(NULL) == NULL));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_array_is_empty(NULL)));
    
    allc_array_delete(&arr);
}

void test__allc__allc_array_modification() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test push operations:\n");
    allc_allocator_t alloc = allc_allocator_system();
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 2);
    
    // Test push
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        allc_array_push(&arr, &values[i]);
        ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(arr.slice.length == (size_t)(i + 1)));
        int *last = (int*)allc_array_last(&arr);
        ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(*last == values[i]));
    }
    
    // Test capacity growth
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(arr.capacity >= 5));
    
    printf(">> Test pop operations:\n");
    int popped;
    bool success = allc_array_pop(&arr, &popped);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(success));
    ALLC_TEST_ANY("50", "%d", popped);
    ALLC_TEST_ANY("4", "%zu", arr.slice.length);
    
    // Test pop without output
    success = allc_array_pop(&arr, NULL);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(success));
    ALLC_TEST_ANY("3", "%zu", arr.slice.length);
    
    printf(">> Test insert operations:\n");
    // Test insert at beginning
    int new_val = 5;
    allc_array_insert(&arr, 0, &new_val);
    ALLC_TEST_ANY("4", "%zu", arr.slice.length);
    int *first = (int*)allc_array_first(&arr);
    ALLC_TEST_ANY("5", "%d", *first);
    
    // Test insert at end
    new_val = 35;
    allc_array_insert(&arr, -1, &new_val);
    ALLC_TEST_ANY("5", "%zu", arr.slice.length);
    
    // Test insert in middle
    new_val = 15;
    allc_array_insert(&arr, 2, &new_val);
    ALLC_TEST_ANY("6", "%zu", arr.slice.length);
    int *middle = (int*)allc_array_at(&arr, 2);
    ALLC_TEST_ANY("15", "%d", *middle);
    
    printf(">> Test remove operations:\n");
    // Test remove
    allc_array_remove(&arr, 0);  // Remove first
    ALLC_TEST_ANY("5", "%zu", arr.slice.length);
    first = (int*)allc_array_first(&arr);
    ALLC_TEST_ANY("10", "%d", *first);
    
    allc_array_remove(&arr, -1); // Remove last
    ALLC_TEST_ANY("4", "%zu", arr.slice.length);
    
    allc_array_remove(&arr, 1);  // Remove middle
    ALLC_TEST_ANY("3", "%zu", arr.slice.length);
    
    printf(">> Test resize operations:\n");
    // Test resize
    allc_array_resize(&arr, 10);
    ALLC_TEST_ANY("10", "%zu", arr.slice.length);
    // New elements should be zero
    for (size_t i = 3; i < 10; i++) {
        int *val = (int*)allc_array_at(&arr, i);
        ALLC_TEST_ANY("0", "%d", *val);
    }
    
    allc_array_resize(&arr, 2);
    ALLC_TEST_ANY("2", "%zu", arr.slice.length);
    
    allc_array_delete(&arr);
}

void test__allc__allc_array_slicing() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test slice operations:\n");
    allc_allocator_t alloc = allc_allocator_system();
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 10);
    
    // Add test data
    for (int i = 1; i <= 10; i++) {
        allc_array_push(&arr, &i);
    }
    
    // Test as_slice
    allc_slice_t full_slice = allc_array_as_slice(&arr);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(full_slice.data == arr.slice.data));
    ALLC_TEST_ANY("10", "%zu", full_slice.length);
    ALLC_TEST_ANY("4", "%zu", full_slice.stride);
    
    printf(">> Test partial slicing:\n");
    // Test array_slice
    allc_slice_t partial = allc_array_slice(&arr, 2, 7);
    ALLC_TEST_ANY("5", "%zu", partial.length);
    int *first_elem = (int*)allc_slice_at(&partial, 0);
    int *last_elem = (int*)allc_slice_at(&partial, -1);
    ALLC_TEST_ANY("3", "%d", *first_elem);
    ALLC_TEST_ANY("7", "%d", *last_elem);
    
    printf(">> Test negative indices:\n");
    allc_slice_t negative = allc_array_slice(&arr, -5, -1);
    ALLC_TEST_ANY("4", "%zu", negative.length);
    first_elem = (int*)allc_slice_at(&negative, 0);
    last_elem = (int*)allc_slice_at(&negative, -1);
    ALLC_TEST_ANY("6", "%d", *first_elem);
    ALLC_TEST_ANY("9", "%d", *last_elem);
    
    printf(">> Test error conditions:\n");
    allc_slice_t null_slice = allc_array_as_slice(NULL);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(null_slice.data == NULL));
    
    null_slice = allc_array_slice(NULL, 0, 5);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(null_slice.data == NULL));
    
    allc_array_delete(&arr);
}

void test__allc__allc_array_edge_cases() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test empty array operations:\n");
    allc_allocator_t alloc = allc_allocator_system();
    
    // Test operations on empty array
    allc_array_t empty = allc_array_new(alloc, sizeof(int), 0);
    
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_array_is_empty(&empty)));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_array_pop(&empty, NULL)));
    
    // Push to empty array should work
    int val = 42;
    allc_array_push(&empty, &val);
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_array_is_empty(&empty)));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(empty.capacity > 0));
    
    printf(">> Test error conditions:\n");
    // Test null safety for all functions - these should not crash
    allc_array_clear(NULL);
    allc_array_push(NULL, &val);
    allc_array_insert(NULL, 0, &val);
    allc_array_remove(NULL, 0);
    allc_array_resize(NULL, 10);
    allc_array_delete(NULL);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(true)); // If we get here, no crash occurred
    
    // Test push with null element
    allc_array_push(&empty, NULL); // Should be safe
    
    // Test insert with null element
    allc_array_insert(&empty, 0, NULL); // Should be safe
    
    printf(">> Test out of bounds operations:\n");
    // Test out of bounds operations - should be safe
    allc_array_remove(&empty, 100);  // Should be safe
    allc_array_remove(&empty, -100); // Should be safe
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(true)); // If we get here, no crash occurred
    
    allc_array_delete(&empty);
}

void test__allc__allc_array_memory_management() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test memory management during growth:\n");
    allc_allocator_t alloc = allc_allocator_system();
    
    // Test that memory is properly managed during growth
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 1);
    
    // Force reallocation
    for (int i = 0; i < 100; i++) {
        allc_array_push(&arr, &i);
    }
    
    // Data pointer may have changed due to reallocation
    ALLC_TEST_ANY("100", "%zu", arr.slice.length);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(arr.capacity >= 100));
    
    printf(">> Test data integrity after reallocation:\n");
    // Verify data integrity
    for (int i = 0; i < 100; i++) {
        int *val = (int*)allc_array_at(&arr, i);
        ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(*val == i));
    }
    
    allc_array_delete(&arr);
}

void test__allc__allc_array_with_custom_types() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test custom struct types:\n");
    allc_allocator_t alloc = allc_allocator_system();
    
    typedef struct {
        int x, y;
        char name[16];
    } Point;
    
    allc_array_t points = allc_array_new(alloc, sizeof(Point), 5);
    
    // Add some points
    Point p1 = {10, 20, "origin"};
    Point p2 = {30, 40, "corner"};
    Point p3 = {50, 60, "center"};
    
    allc_array_push(&points, &p1);
    allc_array_push(&points, &p2);
    allc_array_push(&points, &p3);
    
    ALLC_TEST_ANY("3", "%zu", points.slice.length);
    
    printf(">> Test access and verification:\n");
    // Access and verify
    Point *first = (Point*)allc_array_first(&points);
    Point *last = (Point*)allc_array_last(&points);
    
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(first->x == 10 && first->y == 20));
    ALLC_TEST_ANY("0", "%d", strcmp(first->name, "origin"));
    
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(last->x == 50 && last->y == 60));
    ALLC_TEST_ANY("0", "%d", strcmp(last->name, "center"));
    
    printf(">> Test slice operations with custom types:\n");
    // Test slice operations
    allc_slice_t slice = allc_array_slice(&points, 1, 3);
    ALLC_TEST_ANY("2", "%zu", slice.length);
    
    Point *slice_first = (Point*)allc_slice_at(&slice, 0);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(slice_first->x == 30 && slice_first->y == 40));
    
    allc_array_delete(&points);
}

void test__allc__allc_array_stress() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Test stress - large number of operations:\n");
    allc_allocator_t alloc = allc_allocator_system();
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 1);
    
    // Test large number of operations
    const size_t test_size = 10000;
    
    // Push many elements
    for (size_t i = 0; i < test_size; i++) {
        int val = (int)i;
        allc_array_push(&arr, &val);
    }
    
    ALLC_TEST_ANY("10000", "%zu", arr.slice.length);
    
    printf(">> Test stress - verify all elements:\n");
    // Verify all elements (sample check to avoid too much output)
    for (size_t i = 0; i < test_size; i += 1000) {
        int *val = (int*)allc_array_at(&arr, i);
        ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(val != NULL && *val == (int)i));
    }
    
    printf(">> Test stress - pop operations:\n");
    // Pop half the elements
    bool success = true;
    for (size_t i = 0; success && i < test_size / 2; i++) {
        int val;
        success &= allc_array_pop(&arr, &val);
        success &= (val == (int)(test_size - 1 - i));
    }
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(success));
    ALLC_TEST_ANY("5000", "%zu", arr.slice.length);
    
    printf(">> Test stress - insert operations:\n");
    // Insert elements in the middle
    for (size_t i = 0; i < 100; i++) {
        int val = -((int)i + 1);
        allc_array_insert(&arr, arr.slice.length / 2, &val);
    }
    
    ALLC_TEST_ANY("5100", "%zu", arr.slice.length);
    
    allc_array_delete(&arr);
}

// Main test runner
int main() {
    printf("Running array.h tests...\n");
    printf("========================\n");
    
    // Slice tests
    test__allc__allc_slice_basic();
    test__allc__allc_slice_clear();
    test__allc__allc_slice_copy();
    test__allc__allc_slice_sub();
    
    // Array tests
    test__allc__allc_array_creation();
    test__allc__allc_array_copy();
    test__allc__allc_array_capacity();
    test__allc__allc_array_access();
    test__allc__allc_array_modification();
    test__allc__allc_array_slicing();
    test__allc__allc_array_edge_cases();
    test__allc__allc_array_memory_management();
    test__allc__allc_array_with_custom_types();
    test__allc__allc_array_stress();
    
    printf("========================\n");
    printf("All tests completed!\n");
    
    return 0;
}

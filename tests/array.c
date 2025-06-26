/**
 * Tests for array.h
 * =================
 *
 * Comprehensive tests for dynamic arrays and slices.
 */

#define ALLC_IMPL
#include "../dev/array.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// Test utilities
static void test_report(const char *test_name, bool passed) {
    printf("[%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

// Slice Tests
// ===========

void test_slice_basic() {
    int data[] = {1, 2, 3, 4, 5};
    allc_slice_t slice = {
        .data = data,
        .stride = sizeof(int),
        .length = 5
    };
    
    // Test basic access
    int *first = (int*)allc_slice_at(&slice, 0);
    int *last = (int*)allc_slice_at(&slice, -1);
    int *middle = (int*)allc_slice_at(&slice, 2);
    
    assert(first != NULL && *first == 1);
    assert(last != NULL && *last == 5);
    assert(middle != NULL && *middle == 3);
    
    // Test bounds checking
    assert(allc_slice_at(&slice, 10) == NULL);
    assert(allc_slice_at(&slice, -10) == NULL);
    assert(allc_slice_at(NULL, 0) == NULL);
    
    test_report("slice_basic", true);
}

void test_slice_clear() {
    int data[] = {1, 2, 3, 4, 5};
    allc_slice_t slice = {
        .data = data,
        .stride = sizeof(int),
        .length = 5
    };
    
    allc_slice_clear(&slice);
    
    for (size_t i = 0; i < 5; i++) {
        assert(data[i] == 0);
    }
    
    // Test null safety
    allc_slice_clear(NULL);
    slice.data = NULL;
    allc_slice_clear(&slice);
    
    test_report("slice_clear", true);
}

void test_slice_copy() {
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
    
    for (size_t i = 0; i < 5; i++) {
        assert(dst_data[i] == src_data[i]);
    }
    
    // Test partial copy (dst smaller)
    int small_dst[] = {0, 0, 0};
    allc_slice_t small = {
        .data = small_dst,
        .stride = sizeof(int),
        .length = 3
    };
    
    allc_slice_copy(&small, &src);
    assert(small_dst[0] == 1);
    assert(small_dst[1] == 2);
    assert(small_dst[2] == 3);
    
    // Test null safety
    allc_slice_copy(NULL, &src);
    allc_slice_copy(&dst, NULL);
    
    // Test stride mismatch
    allc_slice_t bad_stride = {
        .data = dst_data,
        .stride = sizeof(char),
        .length = 5
    };
    allc_slice_copy(&bad_stride, &src); // Should do nothing
    
    test_report("slice_copy", true);
}

void test_slice_sub() {
    int data[] = {1, 2, 3, 4, 5};
    allc_slice_t slice = {
        .data = data,
        .stride = sizeof(int),
        .length = 5
    };
    
    // Test normal sub-slice
    allc_slice_t sub1 = allc_slice_sub(&slice, 1, 4);
    assert(sub1.data != NULL);
    assert(sub1.length == 3);
    assert(sub1.stride == sizeof(int));
    assert(*(int*)allc_slice_at(&sub1, 0) == 2);
    assert(*(int*)allc_slice_at(&sub1, -1) == 4);
    
    // Test negative indices
    allc_slice_t sub2 = allc_slice_sub(&slice, -3, -1);
    assert(sub2.data != NULL);
    assert(sub2.length == 2);
    assert(*(int*)allc_slice_at(&sub2, 0) == 3);
    assert(*(int*)allc_slice_at(&sub2, 1) == 4);
    
    // Test empty slice
    allc_slice_t empty = allc_slice_sub(&slice, 2, 2);
    assert(empty.length == 0);
    
    // Test invalid ranges
    allc_slice_t invalid = allc_slice_sub(&slice, 10, 15);
    assert(invalid.length == 0);
    
    // Test null safety
    allc_slice_t null_slice = allc_slice_sub(NULL, 0, 1);
    assert(null_slice.data == NULL);
    assert(null_slice.length == 0);
    
    test_report("slice_sub", true);
}

// Array Tests
// ===========

void test_array_creation() {
    allc_allocator_t alloc = allc_allocator_system();
    
    // Test basic creation
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 10);
    assert(arr.slice.data != NULL);
    assert(arr.slice.stride == sizeof(int));
    assert(arr.slice.length == 0);
    assert(arr.capacity == 10);
    
    allc_array_delete(&arr);
    assert(arr.slice.data == NULL);
    assert(arr.capacity == 0);
    
    // Test zero capacity
    allc_array_t zero_cap = allc_array_new(alloc, sizeof(int), 0);
    assert(zero_cap.slice.data == NULL);
    assert(zero_cap.capacity == 0);
    allc_array_delete(&zero_cap);
    
    test_report("array_creation", true);
}

void test_array_copy() {
    allc_allocator_t alloc = allc_allocator_system();
    
    allc_array_t original = allc_array_new(alloc, sizeof(int), 5);
    
    // Add some data
    for (int i = 1; i <= 5; i++) {
        allc_array_push(&original, &i);
    }
    
    // Test copy
    allc_array_t copy = allc_array_copy(&original);
    assert(copy.slice.length == original.slice.length);
    assert(copy.slice.stride == original.slice.stride);
    assert(copy.slice.data != original.slice.data); // Different memory
    
    for (size_t i = 0; i < copy.slice.length; i++) {
        int *orig_val = (int*)allc_array_at(&original, i);
        int *copy_val = (int*)allc_array_at(&copy, i);
        assert(*orig_val == *copy_val);
    }
    
    // Test null copy
    allc_array_t null_copy = allc_array_copy(NULL);
    assert(null_copy.slice.data == NULL);
    assert(null_copy.slice.length == 0);
    
    allc_array_delete(&original);
    allc_array_delete(&copy);
    
    test_report("array_copy", true);
}

void test_array_capacity() {
    allc_allocator_t alloc = allc_allocator_system();
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 5);
    
    // Test grow
    size_t old_capacity = arr.capacity;
    allc_array_grow(&arr, 10);
    assert(arr.capacity == old_capacity + 10);
    
    // Test ensure_capacity (no growth needed)
    allc_array_ensure_capacity(&arr, 10);
    assert(arr.capacity >= 15); // Should be at least 15
    
    // Test ensure_capacity (growth needed)
    size_t current_capacity = arr.capacity;
    allc_array_ensure_capacity(&arr, current_capacity + 20);
    assert(arr.capacity >= current_capacity + 20);
    
    // Test reserve
    current_capacity = arr.capacity;
    arr.slice.length = 5;
    allc_array_reserve(&arr, 10);
    assert(arr.capacity >= current_capacity || arr.capacity >= 15);
    
    // Test null safety
    allc_array_grow(NULL, 10);
    allc_array_ensure_capacity(NULL, 10);
    allc_array_reserve(NULL, 10);
    
    allc_array_delete(&arr);
    test_report("array_capacity", true);
}

void test_array_access() {
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
    
    assert(first != NULL && *first == 1);
    assert(last != NULL && *last == 5);
    assert(at_index != NULL && *at_index == 3);
    
    // Test negative indexing
    int *from_end = (int*)allc_array_at(&arr, -2);
    assert(from_end != NULL && *from_end == 4);
    
    // Test bounds
    assert(allc_array_at(&arr, 10) == NULL);
    assert(allc_array_at(&arr, -10) == NULL);
    
    // Test empty array
    assert(!allc_array_is_empty(&arr));
    allc_array_clear(&arr);
    assert(allc_array_is_empty(&arr));
    assert(allc_array_first(&arr) == NULL);
    assert(allc_array_last(&arr) == NULL);
    
    // Test null safety
    assert(allc_array_at(NULL, 0) == NULL);
    assert(allc_array_first(NULL) == NULL);
    assert(allc_array_last(NULL) == NULL);
    assert(allc_array_is_empty(NULL));
    
    allc_array_delete(&arr);
    test_report("array_access", true);
}

void test_array_modification() {
    allc_allocator_t alloc = allc_allocator_system();
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 2);
    
    // Test push
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        allc_array_push(&arr, &values[i]);
        assert(arr.slice.length == (size_t)(i + 1));
        int *last = (int*)allc_array_last(&arr);
        assert(*last == values[i]);
    }
    
    // Test capacity growth
    assert(arr.capacity >= 5);
    
    // Test pop
    int popped;
    bool success = allc_array_pop(&arr, &popped);
    assert(success);
    assert(popped == 50);
    assert(arr.slice.length == 4);
    
    // Test pop without output
    success = allc_array_pop(&arr, NULL);
    assert(success);
    assert(arr.slice.length == 3);
    
    // Test insert at beginning
    int new_val = 5;
    allc_array_insert(&arr, 0, &new_val);
    assert(arr.slice.length == 4);
    int *first = (int*)allc_array_first(&arr);
    assert(*first == 5);
    
    // Test insert at end
    new_val = 35;
    allc_array_insert(&arr, -1, &new_val);
    assert(arr.slice.length == 5);
    
    // Test insert in middle
    new_val = 15;
    allc_array_insert(&arr, 2, &new_val);
    assert(arr.slice.length == 6);
    int *middle = (int*)allc_array_at(&arr, 2);
    assert(*middle == 15);
    
    // Test remove
    allc_array_remove(&arr, 0);  // Remove first
    assert(arr.slice.length == 5);
    first = (int*)allc_array_first(&arr);
    assert(*first == 10);
    
    allc_array_remove(&arr, -1); // Remove last
    assert(arr.slice.length == 4);
    
    allc_array_remove(&arr, 1);  // Remove middle
    assert(arr.slice.length == 3);
    
    // Test resize
    allc_array_resize(&arr, 10);
    assert(arr.slice.length == 10);
    // New elements should be zero
    for (size_t i = 3; i < 10; i++) {
        int *val = (int*)allc_array_at(&arr, i);
        assert(*val == 0);
    }
    
    allc_array_resize(&arr, 2);
    assert(arr.slice.length == 2);
    
    allc_array_delete(&arr);
    test_report("array_modification", true);
}

void test_array_slicing() {
    allc_allocator_t alloc = allc_allocator_system();
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 10);
    
    // Add test data
    for (int i = 1; i <= 10; i++) {
        allc_array_push(&arr, &i);
    }
    
    // Test as_slice
    allc_slice_t full_slice = allc_array_as_slice(&arr);
    assert(full_slice.data == arr.slice.data);
    assert(full_slice.length == arr.slice.length);
    assert(full_slice.stride == arr.slice.stride);
    
    // Test array_slice
    allc_slice_t partial = allc_array_slice(&arr, 2, 7);
    assert(partial.length == 5);
    int *first_elem = (int*)allc_slice_at(&partial, 0);
    int *last_elem = (int*)allc_slice_at(&partial, -1);
    assert(*first_elem == 3);
    assert(*last_elem == 7);
    
    // Test negative indices
    allc_slice_t negative = allc_array_slice(&arr, -5, -1);
    assert(negative.length == 4);
    first_elem = (int*)allc_slice_at(&negative, 0);
    last_elem = (int*)allc_slice_at(&negative, -1);
    assert(*first_elem == 6);
    assert(*last_elem == 9);
    
    // Test null safety
    allc_slice_t null_slice = allc_array_as_slice(NULL);
    assert(null_slice.data == NULL);
    
    null_slice = allc_array_slice(NULL, 0, 5);
    assert(null_slice.data == NULL);
    
    allc_array_delete(&arr);
    test_report("array_slicing", true);
}

void test_array_edge_cases() {
    allc_allocator_t alloc = allc_allocator_system();
    
    // Test operations on empty array
    allc_array_t empty = allc_array_new(alloc, sizeof(int), 0);
    
    assert(allc_array_is_empty(&empty));
    assert(allc_array_pop(&empty, NULL) == false);
    
    // Push to empty array should work
    int val = 42;
    allc_array_push(&empty, &val);
    assert(!allc_array_is_empty(&empty));
    assert(empty.capacity > 0);
    
    // Test null safety for all functions
    allc_array_clear(NULL);
    allc_array_push(NULL, &val);
    allc_array_insert(NULL, 0, &val);
    allc_array_remove(NULL, 0);
    allc_array_resize(NULL, 10);
    allc_array_delete(NULL);
    
    // Test push with null element
    allc_array_push(&empty, NULL); // Should be safe
    
    // Test insert with null element
    allc_array_insert(&empty, 0, NULL); // Should be safe
    
    // Test out of bounds operations
    allc_array_remove(&empty, 100);  // Should be safe
    allc_array_remove(&empty, -100); // Should be safe
    
    allc_array_delete(&empty);
    test_report("array_edge_cases", true);
}

void test_array_memory_management() {
    allc_allocator_t alloc = allc_allocator_system();
    
    // Test that memory is properly managed during growth
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 1);
    
    // Force reallocation
    for (int i = 0; i < 100; i++) {
        allc_array_push(&arr, &i);
    }
    
    // Data pointer may have changed due to reallocation
    assert(arr.slice.length == 100);
    assert(arr.capacity >= 100);
    
    // Verify data integrity
    for (int i = 0; i < 100; i++) {
        int *val = (int*)allc_array_at(&arr, i);
        assert(*val == i);
    }
    
    allc_array_delete(&arr);
    test_report("array_memory_management", true);
}

void test_array_with_custom_types() {
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
    
    assert(points.slice.length == 3);
    
    // Access and verify
    Point *first = (Point*)allc_array_first(&points);
    Point *last = (Point*)allc_array_last(&points);
    
    assert(first->x == 10 && first->y == 20);
    assert(strcmp(first->name, "origin") == 0);
    
    assert(last->x == 50 && last->y == 60);
    assert(strcmp(last->name, "center") == 0);
    
    // Test slice operations
    allc_slice_t slice = allc_array_slice(&points, 1, 3);
    assert(slice.length == 2);
    
    Point *slice_first = (Point*)allc_slice_at(&slice, 0);
    assert(slice_first->x == 30 && slice_first->y == 40);
    
    allc_array_delete(&points);
    test_report("array_with_custom_types", true);
}

void test_array_stress() {
    allc_allocator_t alloc = allc_allocator_system();
    allc_array_t arr = allc_array_new(alloc, sizeof(int), 1);
    
    // Test large number of operations
    const size_t test_size = 10000;
    
    // Push many elements
    for (size_t i = 0; i < test_size; i++) {
        int val = (int)i;
        allc_array_push(&arr, &val);
    }
    
    assert(arr.slice.length == test_size);
    
    // Verify all elements
    for (size_t i = 0; i < test_size; i++) {
        int *val = (int*)allc_array_at(&arr, i);
        assert(val != NULL && *val == (int)i);
    }
    
    // Pop half the elements
    for (size_t i = 0; i < test_size / 2; i++) {
        int val;
        bool success = allc_array_pop(&arr, &val);
        assert(success);
        assert(val == (int)(test_size - 1 - i));
    }
    
    assert(arr.slice.length == test_size / 2);
    
    // Insert elements in the middle
    for (size_t i = 0; i < 100; i++) {
        int val = -((int)i + 1);
        allc_array_insert(&arr, arr.slice.length / 2, &val);
    }
    
    assert(arr.slice.length == test_size / 2 + 100);
    
    allc_array_delete(&arr);
    test_report("array_stress", true);
}

// Main test runner
int main() {
    printf("Running array.h tests...\n");
    printf("========================\n");
    
    // Slice tests
    test_slice_basic();
    test_slice_clear();
    test_slice_copy();
    test_slice_sub();
    
    // Array tests
    test_array_creation();
    test_array_copy();
    test_array_capacity();
    test_array_access();
    test_array_modification();
    test_array_slicing();
    test_array_edge_cases();
    test_array_memory_management();
    test_array_with_custom_types();
    test_array_stress();
    
    printf("========================\n");
    printf("All tests completed!\n");
    
    return 0;
}

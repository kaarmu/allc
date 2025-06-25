#define ALLC_IMPL

#include "../dev/allocator.h"
#include "../dev/macro.h"

#include <stdio.h>
#include <string.h>

void test__allc_allocator_system(void) {
    printf("\n[%s]\n", __func__);
    
    // Test system allocator creation
    allc_allocator_t allocator = allc_allocator_system();
    ALLC_TEST_BOOL(true, allocator != NULL);
    
    // Test convenience alias
    allc_allocator_t allocator_c = allc_allocator_c();
    ALLC_TEST_BOOL(true, allocator_c == allocator); // Should be same instance
    
    // Test reset capability
    ALLC_TEST_BOOL(false, allc_allocator_can_reset(allocator));
    
    // Test basic allocation
    void *ptr = allc_allocator_alloc(allocator, 100);
    ALLC_TEST_BOOL(true, ptr != NULL);
    
    // Test free
    allc_allocator_free(allocator, ptr);
    
    // Test realloc
    ptr = allc_allocator_alloc(allocator, 50);
    ALLC_TEST_BOOL(true, ptr != NULL);
    
    void *new_ptr = allc_allocator_realloc(allocator, ptr, 200);
    ALLC_TEST_BOOL(true, new_ptr != NULL);
    
    allc_allocator_free(allocator, new_ptr);
    
    // Test delete (should be no-op for system allocator)
    allc_allocator_delete(allocator);
}

void test__allc_allocator_bump_new(void) {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t parent = allc_allocator_system();
    allc_allocator_t bump = allc_allocator_bump_new(parent, 1024);
    
    ALLC_TEST_BOOL(true, bump != NULL);
    ALLC_TEST_BOOL(true, allc_allocator_can_reset(bump));
    
    // Test basic allocation
    void *ptr1 = allc_allocator_alloc(bump, 100);
    ALLC_TEST_BOOL(true, ptr1 != NULL);
    
    void *ptr2 = allc_allocator_alloc(bump, 200);
    ALLC_TEST_BOOL(true, ptr2 != NULL);
    ALLC_TEST_BOOL(true, ptr2 > ptr1); // Should be sequential
    
    // Test stats
    allc_allocator_stats_t stats = allc_allocator_get_stats(bump);
    ALLC_TEST_BOOL(true, stats.total_size == 1024);
    ALLC_TEST_BOOL(true, stats.used_size > 0);
    ALLC_TEST_BOOL(true, stats.allocations == 2);
    ALLC_TEST_BOOL(true, stats.can_reset);
    
    // Test reset
    allc_allocator_reset(bump);
    stats = allc_allocator_get_stats(bump);
    ALLC_TEST_BOOL(true, stats.used_size == 0);
    ALLC_TEST_BOOL(true, stats.allocations == 0);
    
    // Test allocation after reset
    void *ptr3 = allc_allocator_alloc(bump, 50);
    ALLC_TEST_BOOL(true, ptr3 != NULL);
    ALLC_TEST_BOOL(true, ptr3 == ptr1); // Should reuse space
    
    // Test out of memory
    void *big_ptr = allc_allocator_alloc(bump, 2000);
    ALLC_TEST_BOOL(true, big_ptr == NULL);
    
    allc_allocator_delete(bump);
}

void test__allc_allocator_bump_from_buffer(void) {
    printf("\n[%s]\n", __func__);
    
    char buffer[512];
    allc_allocator_t bump = allc_allocator_bump_from_buffer(buffer, sizeof(buffer));
    
    ALLC_TEST_BOOL(true, bump != NULL);
    ALLC_TEST_BOOL(true, allc_allocator_can_reset(bump));
    
    // Test allocation
    void *ptr = allc_allocator_alloc(bump, 100);
    ALLC_TEST_BOOL(true, ptr != NULL);
    
    // Test stats
    allc_allocator_stats_t stats = allc_allocator_get_stats(bump);
    ALLC_TEST_BOOL(true, stats.total_size < sizeof(buffer)); // Less due to allocator overhead
    ALLC_TEST_BOOL(true, stats.used_size > 0);
    ALLC_TEST_BOOL(true, stats.allocations == 1);
    
    // Test reset
    allc_allocator_reset(bump);
    stats = allc_allocator_get_stats(bump);
    ALLC_TEST_BOOL(true, stats.used_size == 0);
    ALLC_TEST_BOOL(true, stats.allocations == 0);
    
    // Test delete (should be no-op for buffer-based allocator)
    allc_allocator_delete(bump);
}

void test__allc_allocator_buffer_8bit(void) {
    printf("\n[%s]\n", __func__);
    
    allc_u8_t buffer[128];
    allc_allocator_t buf_alloc = allc_allocator_buffer_8bit(buffer, sizeof(buffer));
    
    ALLC_TEST_BOOL(true, buf_alloc != NULL);
    ALLC_TEST_BOOL(true, allc_allocator_can_reset(buf_alloc));
    
    // Test allocation
    void *ptr1 = allc_allocator_alloc(buf_alloc, 10);
    ALLC_TEST_BOOL(true, ptr1 != NULL);
    
    void *ptr2 = allc_allocator_alloc(buf_alloc, 20);
    ALLC_TEST_BOOL(true, ptr2 != NULL);
    
    // Test large allocation (should fail)
    void *big_ptr = allc_allocator_alloc(buf_alloc, 300);
    ALLC_TEST_BOOL(true, big_ptr == NULL);
    
    // Test stats
    allc_allocator_stats_t stats = allc_allocator_get_stats(buf_alloc);
    ALLC_TEST_BOOL(true, stats.total_size < 128); // Less due to overhead
    ALLC_TEST_BOOL(true, stats.used_size > 0);
    ALLC_TEST_BOOL(true, stats.allocations == 2);
    
    // Test reset
    allc_allocator_reset(buf_alloc);
    stats = allc_allocator_get_stats(buf_alloc);
    ALLC_TEST_BOOL(true, stats.used_size == 0);
    ALLC_TEST_BOOL(true, stats.allocations == 0);
}

void test__allc_allocator_buffer_16bit(void) {
    printf("\n[%s]\n", __func__);
    
    allc_u16_t buffer[256];
    allc_allocator_t buf_alloc = allc_allocator_buffer_16bit(buffer, sizeof(buffer)/sizeof(allc_u16_t));
    
    ALLC_TEST_BOOL(true, buf_alloc != NULL);
    ALLC_TEST_BOOL(true, allc_allocator_can_reset(buf_alloc));
    
    // Test allocation
    void *ptr = allc_allocator_alloc(buf_alloc, 100);
    ALLC_TEST_BOOL(true, ptr != NULL);
    
    // Test stats
    allc_allocator_stats_t stats = allc_allocator_get_stats(buf_alloc);
    ALLC_TEST_BOOL(true, stats.total_size > 0);
    ALLC_TEST_BOOL(true, stats.used_size > 0);
    ALLC_TEST_BOOL(true, stats.allocations == 1);
    
    // Test reset
    allc_allocator_reset(buf_alloc);
    stats = allc_allocator_get_stats(buf_alloc);
    ALLC_TEST_BOOL(true, stats.used_size == 0);
}

void test__allc_allocator_buffer_32bit(void) {
    printf("\n[%s]\n", __func__);
    
    allc_u32_t buffer[256]; // 1KB buffer
    allc_allocator_t allocator = allc_allocator_buffer_32bit(buffer, 256);
    
    ALLC_TEST_BOOL(true, allocator != NULL);
    ALLC_TEST_BOOL(true, allc_allocator_can_reset(allocator));
    
    // Test basic allocation
    void *ptr1 = allc_allocator_alloc(allocator, 32);
    ALLC_TEST_BOOL(true, ptr1 != NULL);
    
    void *ptr2 = allc_allocator_alloc(allocator, 64);
    ALLC_TEST_BOOL(true, ptr2 != NULL);
    ALLC_TEST_BOOL(true, ptr2 != ptr1);
    
    // Test reset functionality
    allc_allocator_reset(allocator);
    void *ptr3 = allc_allocator_alloc(allocator, 32);
    ALLC_TEST_BOOL(true, ptr3 == ptr1); // Should reuse first location after reset
    
    // Test stats
    allc_allocator_stats_t stats = allc_allocator_get_stats(allocator);
    ALLC_TEST_BOOL(true, stats.can_reset);
    ALLC_TEST_BOOL(true, stats.total_size > 0);
    ALLC_TEST_BOOL(true, stats.used_size > 0);
    ALLC_TEST_ANY("1", "%zu", stats.allocations);
}

void test__allc_allocator_buffer_64bit(void) {
    printf("\n[%s]\n", __func__);
    
    allc_u64_t buffer[128]; // 1KB buffer
    allc_allocator_t allocator = allc_allocator_buffer_64bit(buffer, 128);
    
    ALLC_TEST_BOOL(true, allocator != NULL);
    ALLC_TEST_BOOL(true, allc_allocator_can_reset(allocator));
    
    // Test basic allocation
    void *ptr1 = allc_allocator_alloc(allocator, 64);
    ALLC_TEST_BOOL(true, ptr1 != NULL);
    
    void *ptr2 = allc_allocator_alloc(allocator, 128);
    ALLC_TEST_BOOL(true, ptr2 != NULL);
    ALLC_TEST_BOOL(true, ptr2 != ptr1);
    
    // Test reset functionality
    allc_allocator_reset(allocator);
    void *ptr3 = allc_allocator_alloc(allocator, 64);
    ALLC_TEST_BOOL(true, ptr3 == ptr1); // Should reuse first location after reset
    
    // Test stats
    allc_allocator_stats_t stats = allc_allocator_get_stats(allocator);
    ALLC_TEST_BOOL(true, stats.can_reset);
    ALLC_TEST_BOOL(true, stats.total_size > 0);
    ALLC_TEST_BOOL(true, stats.used_size > 0);
    ALLC_TEST_ANY("1", "%zu", stats.allocations);
}

void test__allc_allocator_macros(void) {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_system();
    
    // Test ALLC_ALLOC macro
    int *int_ptr = ALLC_ALLOC(allocator, int);
    ALLC_TEST_BOOL(true, int_ptr != NULL);
    *int_ptr = 42;
    ALLC_TEST_ANY("42", "%d", *int_ptr);
    
    // Test ALLC_ALLOC_ARRAY macro
    float *float_array = ALLC_ALLOC_ARRAY(allocator, float, 10);
    ALLC_TEST_BOOL(true, float_array != NULL);
    float_array[0] = 3.14f;
    float_array[9] = 2.71f;
    ALLC_TEST_BOOL(true, float_array[0] > 3.0f);
    ALLC_TEST_BOOL(true, float_array[9] > 2.0f);
    
    // Test ALLC_FREE macro
    ALLC_FREE(allocator, int_ptr);
    ALLC_FREE(allocator, float_array);
}

void test__allc_allocator_error_conditions(void) {
    printf("\n[%s]\n", __func__);
    
    // Test NULL allocator
    void *ptr = allc_allocator_alloc(NULL, 100);
    ALLC_TEST_BOOL(true, ptr == NULL);
    
    allc_allocator_free(NULL, (void*)0x1234);
    allc_allocator_reset(NULL);
    ALLC_TEST_BOOL(false, allc_allocator_can_reset(NULL));
    
    // Test NULL buffer for bump allocator
    allc_allocator_t bump = allc_allocator_bump_from_buffer(NULL, 1024);
    ALLC_TEST_BOOL(true, bump == NULL);
    
    // Test too small buffer
    char small_buffer[10];
    bump = allc_allocator_bump_from_buffer(small_buffer, sizeof(small_buffer));
    ALLC_TEST_BOOL(true, bump == NULL);
    
    // Test NULL buffer for 8-bit allocator
    allc_allocator_t buf8 = allc_allocator_buffer_8bit(NULL, 100);
    ALLC_TEST_BOOL(true, buf8 == NULL);
    
    // Test zero allocation
    allc_allocator_t sys = allc_allocator_system();
    ptr = allc_allocator_alloc(sys, 0);
    // Behavior is implementation-defined, but shouldn't crash
    if (ptr) {
        allc_allocator_free(sys, ptr);
    }
}

void test__allc_allocator_alignment(void) {
    printf("\n[%s]\n", __func__);
    
    char buffer[1024];
    allc_allocator_t bump = allc_allocator_bump_from_buffer(buffer, sizeof(buffer));
    
    // Test that allocations are properly aligned
    void *ptr1 = allc_allocator_alloc(bump, 1);
    void *ptr2 = allc_allocator_alloc(bump, 1);
    void *ptr3 = allc_allocator_alloc(bump, 1);
    
    ALLC_TEST_BOOL(true, ptr1 != NULL);
    ALLC_TEST_BOOL(true, ptr2 != NULL);
    ALLC_TEST_BOOL(true, ptr3 != NULL);
    
    // Check alignment (pointers should be aligned to pointer size)
    ALLC_TEST_BOOL(true, ((uintptr_t)ptr1 % sizeof(void*)) == 0);
    ALLC_TEST_BOOL(true, ((uintptr_t)ptr2 % sizeof(void*)) == 0);
    ALLC_TEST_BOOL(true, ((uintptr_t)ptr3 % sizeof(void*)) == 0);
}

void test__allc_allocator_stats(void) {
    printf("\n[%s]\n", __func__);
    
    // Test stats for system allocator
    allc_allocator_t sys = allc_allocator_system();
    allc_allocator_stats_t sys_stats = allc_allocator_get_stats(sys);
    ALLC_TEST_BOOL(false, sys_stats.can_reset);
    ALLC_TEST_ANY("0", "%zu", sys_stats.total_size); // Unknown for system allocator
    
    // Test stats for bump allocator
    allc_allocator_t bump = allc_allocator_bump_new(sys, 500);
    allc_allocator_stats_t bump_stats = allc_allocator_get_stats(bump);
    
    ALLC_TEST_BOOL(true, bump_stats.can_reset);
    ALLC_TEST_ANY("500", "%zu", bump_stats.total_size);
    ALLC_TEST_ANY("0", "%zu", bump_stats.used_size);
    ALLC_TEST_ANY("500", "%zu", bump_stats.free_size);
    ALLC_TEST_ANY("0", "%zu", bump_stats.allocations);
    
    // Allocate something and check stats again
    void *ptr = allc_allocator_alloc(bump, 100);
    ALLC_TEST_BOOL(true, ptr != NULL);
    
    bump_stats = allc_allocator_get_stats(bump);
    ALLC_TEST_BOOL(true, bump_stats.used_size > 0);
    ALLC_TEST_BOOL(true, bump_stats.free_size < 500);
    ALLC_TEST_ANY("1", "%zu", bump_stats.allocations);
    
    allc_allocator_delete(bump);
    
    // Test stats for NULL allocator
    allc_allocator_stats_t null_stats = allc_allocator_get_stats(NULL);
    ALLC_TEST_ANY("0", "%zu", null_stats.total_size);
    ALLC_TEST_BOOL(false, null_stats.can_reset);
}

int main(void) {
    printf("=== allc allocator.h Test Suite ===\n");
    
    test__allc_allocator_system();
    test__allc_allocator_bump_new();
    test__allc_allocator_bump_from_buffer();
    test__allc_allocator_buffer_8bit();
    test__allc_allocator_buffer_16bit();
    test__allc_allocator_buffer_32bit();
    test__allc_allocator_buffer_64bit();
    test__allc_allocator_macros();
    test__allc_allocator_error_conditions();
    test__allc_allocator_alignment();
    test__allc_allocator_stats();
    
    printf("\n=== All tests completed ===\n");
    return 0;
}

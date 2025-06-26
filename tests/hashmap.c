/**
 * Tests for hashmap.h
 * ===================
 *
 * Comprehensive tests for hash maps and all functionality.
 */

#define ALLC_IMPL
#include "../dev/hashmap.h"
#include "../dev/macro.h"
#include <stdio.h>
#include <string.h>

// Basic Hash Map Tests
// ====================

void test__allc_hashmap_creation_deletion() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    
    // Test string hashmap creation
    allc_hashmap_t str_map = allc_hashmap_str_new(allocator, sizeof(int));
    ALLC_TEST_BOOL(true, str_map.buckets != NULL);
    ALLC_TEST_ANY("0", "%zu", allc_hashmap_size(&str_map));
    ALLC_TEST_BOOL(true, allc_hashmap_is_empty(&str_map));
    ALLC_TEST_BOOL(true, allc_hashmap_bucket_count(&str_map) >= ALLC_HASHMAP_DEFAULT_CAPACITY);
    
    allc_hashmap_delete(&str_map);
    ALLC_TEST_BOOL(true, str_map.buckets == NULL);
    
    // Test integer hashmap creation
    allc_hashmap_t int_map = allc_hashmap_int_new(allocator, sizeof(float));
    ALLC_TEST_BOOL(true, int_map.buckets != NULL);
    ALLC_TEST_ANY("0", "%zu", allc_hashmap_size(&int_map));
    ALLC_TEST_BOOL(true, allc_hashmap_is_empty(&int_map));
    
    allc_hashmap_delete(&int_map);
    
    // Test custom hashmap creation
    allc_hashmap_t custom_map = allc_hashmap_new(allocator, sizeof(double),
                                               allc_hashmap_hash_bytes,
                                               allc_hashmap_equal_bytes,
                                               NULL, NULL);
    ALLC_TEST_BOOL(true, custom_map.buckets != NULL);
    
    allc_hashmap_delete(&custom_map);
}

void test__allc_hashmap_with_capacity() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    
    // Test creation with custom capacity
    allc_hashmap_t map = allc_hashmap_new_with_capacity(allocator, sizeof(int), 64, 0.8f,
                                                       allc_hashmap_hash_string,
                                                       allc_hashmap_equal_string,
                                                       NULL, NULL);
    
    ALLC_TEST_BOOL(true, map.buckets != NULL);
    ALLC_TEST_BOOL(true, allc_hashmap_bucket_count(&map) >= 64);
    ALLC_TEST_ANY("0.8", "%.1f", map.load_factor);
    
    allc_hashmap_delete(&map);
}

void test__allc_hashmap_str_operations() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_hashmap_t map = allc_hashmap_str_new(allocator, sizeof(int));
    
    // Test put operations
    int value1 = 42, value2 = 100, value3 = 999;
    
    ALLC_TEST_ANY("0", "%d", allc_hashmap_str_put(&map, "answer", &value1));
    ALLC_TEST_ANY("1", "%zu", allc_hashmap_size(&map));
    ALLC_TEST_BOOL(false, allc_hashmap_is_empty(&map));
    
    ALLC_TEST_ANY("0", "%d", allc_hashmap_str_put(&map, "hundred", &value2));
    ALLC_TEST_ANY("2", "%zu", allc_hashmap_size(&map));
    
    ALLC_TEST_ANY("0", "%d", allc_hashmap_str_put(&map, "big", &value3));
    ALLC_TEST_ANY("3", "%zu", allc_hashmap_size(&map));
    
    // Test get operations
    int *result1 = (int*)allc_hashmap_str_get(&map, "answer");
    int *result2 = (int*)allc_hashmap_str_get(&map, "hundred");
    int *result3 = (int*)allc_hashmap_str_get(&map, "big");
    int *result4 = (int*)allc_hashmap_str_get(&map, "nonexistent");
    
    ALLC_TEST_BOOL(true, result1 != NULL);
    ALLC_TEST_ANY("42", "%d", result1 ? *result1 : -1);
    
    ALLC_TEST_BOOL(true, result2 != NULL);
    ALLC_TEST_ANY("100", "%d", result2 ? *result2 : -1);
    
    ALLC_TEST_BOOL(true, result3 != NULL);
    ALLC_TEST_ANY("999", "%d", result3 ? *result3 : -1);
    
    ALLC_TEST_BOOL(true, result4 == NULL);
    
    // Test has operations
    ALLC_TEST_BOOL(true, allc_hashmap_str_has(&map, "answer"));
    ALLC_TEST_BOOL(true, allc_hashmap_str_has(&map, "hundred"));
    ALLC_TEST_BOOL(true, allc_hashmap_str_has(&map, "big"));
    ALLC_TEST_BOOL(false, allc_hashmap_str_has(&map, "nonexistent"));
    
    // Test update existing key
    int new_value = 999999;
    ALLC_TEST_ANY("0", "%d", allc_hashmap_str_put(&map, "answer", &new_value));
    ALLC_TEST_ANY("3", "%zu", allc_hashmap_size(&map)); // Size shouldn't change
    
    result1 = (int*)allc_hashmap_str_get(&map, "answer");
    ALLC_TEST_ANY("999999", "%d", result1 ? *result1 : -1);
    
    allc_hashmap_delete(&map);
}

void test__allc_hashmap_int_operations() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_hashmap_t map = allc_hashmap_int_new(allocator, sizeof(float));
    
    // Test put operations
    float value1 = 3.14f, value2 = 2.71f, value3 = 1.41f;
    
    ALLC_TEST_ANY("0", "%d", allc_hashmap_int_put(&map, 42, &value1));
    ALLC_TEST_ANY("0", "%d", allc_hashmap_int_put(&map, 100, &value2));
    ALLC_TEST_ANY("0", "%d", allc_hashmap_int_put(&map, -50, &value3));
    
    ALLC_TEST_ANY("3", "%zu", allc_hashmap_size(&map));
    
    // Test get operations
    float *result1 = (float*)allc_hashmap_int_get(&map, 42);
    float *result2 = (float*)allc_hashmap_int_get(&map, 100);
    float *result3 = (float*)allc_hashmap_int_get(&map, -50);
    float *result4 = (float*)allc_hashmap_int_get(&map, 999);
    
    ALLC_TEST_BOOL(true, result1 != NULL);
    ALLC_TEST_ANY("3.14", "%.2f", result1 ? *result1 : 0.0f);
    
    ALLC_TEST_BOOL(true, result2 != NULL);
    ALLC_TEST_ANY("2.71", "%.2f", result2 ? *result2 : 0.0f);
    
    ALLC_TEST_BOOL(true, result3 != NULL);
    ALLC_TEST_ANY("1.41", "%.2f", result3 ? *result3 : 0.0f);
    
    ALLC_TEST_BOOL(true, result4 == NULL);
    
    // Test has operations
    ALLC_TEST_BOOL(true, allc_hashmap_int_has(&map, 42));
    ALLC_TEST_BOOL(true, allc_hashmap_int_has(&map, 100));
    ALLC_TEST_BOOL(true, allc_hashmap_int_has(&map, -50));
    ALLC_TEST_BOOL(false, allc_hashmap_int_has(&map, 999));
    
    allc_hashmap_delete(&map);
}

void test__allc_hashmap_remove_operations() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_hashmap_t map = allc_hashmap_str_new(allocator, sizeof(int));
    
    // Add some entries
    int value1 = 1, value2 = 2, value3 = 3;
    allc_hashmap_str_put(&map, "one", &value1);
    allc_hashmap_str_put(&map, "two", &value2);
    allc_hashmap_str_put(&map, "three", &value3);
    
    ALLC_TEST_ANY("3", "%zu", allc_hashmap_size(&map));
    
    // Test successful removal
    ALLC_TEST_ANY("0", "%d", allc_hashmap_str_remove(&map, "two"));
    ALLC_TEST_ANY("2", "%zu", allc_hashmap_size(&map));
    ALLC_TEST_BOOL(false, allc_hashmap_str_has(&map, "two"));
    ALLC_TEST_BOOL(true, allc_hashmap_str_has(&map, "one"));
    ALLC_TEST_BOOL(true, allc_hashmap_str_has(&map, "three"));
    
    // Test removal of non-existent key
    ALLC_TEST_ANY("3", "%d", allc_hashmap_str_remove(&map, "nonexistent"));
    ALLC_TEST_ANY("2", "%zu", allc_hashmap_size(&map));
    
    // Remove all remaining entries
    ALLC_TEST_ANY("0", "%d", allc_hashmap_str_remove(&map, "one"));
    ALLC_TEST_ANY("0", "%d", allc_hashmap_str_remove(&map, "three"));
    ALLC_TEST_ANY("0", "%zu", allc_hashmap_size(&map));
    ALLC_TEST_BOOL(true, allc_hashmap_is_empty(&map));
    
    allc_hashmap_delete(&map);
}

void test__allc_hashmap_clear() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_hashmap_t map = allc_hashmap_str_new(allocator, sizeof(int));
    
    // Add multiple entries
    for (int i = 0; i < 10; i++) {
        char key[32];
        sprintf(key, "key_%d", i);
        allc_hashmap_str_put(&map, key, &i);
    }
    
    ALLC_TEST_ANY("10", "%zu", allc_hashmap_size(&map));
    ALLC_TEST_BOOL(false, allc_hashmap_is_empty(&map));
    
    // Clear the map
    allc_hashmap_clear(&map);
    
    ALLC_TEST_ANY("0", "%zu", allc_hashmap_size(&map));
    ALLC_TEST_BOOL(true, allc_hashmap_is_empty(&map));
    
    // Verify no entries remain
    for (int i = 0; i < 10; i++) {
        char key[32];
        sprintf(key, "key_%d", i);
        ALLC_TEST_BOOL(false, allc_hashmap_str_has(&map, key));
    }
    
    allc_hashmap_delete(&map);
}

void test__allc_hashmap_copy() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_hashmap_t original = allc_hashmap_str_new(allocator, sizeof(int));
    
    // Add entries to original
    int value1 = 100, value2 = 200, value3 = 300;
    allc_hashmap_str_put(&original, "first", &value1);
    allc_hashmap_str_put(&original, "second", &value2);
    allc_hashmap_str_put(&original, "third", &value3);
    
    // Create copy
    allc_hashmap_t copy = allc_hashmap_copy(&original);
    
    ALLC_TEST_BOOL(true, copy.buckets != NULL);
    ALLC_TEST_ANY("3", "%zu", allc_hashmap_size(&copy));
    ALLC_TEST_BOOL(true, copy.buckets != original.buckets); // Different memory
    
    // Verify all entries copied correctly
    int *result1 = (int*)allc_hashmap_str_get(&copy, "first");
    int *result2 = (int*)allc_hashmap_str_get(&copy, "second");
    int *result3 = (int*)allc_hashmap_str_get(&copy, "third");
    
    ALLC_TEST_ANY("100", "%d", result1 ? *result1 : -1);
    ALLC_TEST_ANY("200", "%d", result2 ? *result2 : -1);
    ALLC_TEST_ANY("300", "%d", result3 ? *result3 : -1);
    
    // Modify original, copy should be unchanged
    int new_value = 999;
    allc_hashmap_str_put(&original, "first", &new_value);
    
    result1 = (int*)allc_hashmap_str_get(&copy, "first");
    ALLC_TEST_ANY("100", "%d", result1 ? *result1 : -1); // Copy unchanged
    
    allc_hashmap_delete(&original);
    allc_hashmap_delete(&copy);
}

// Iterator Tests
// ==============

void test__allc_hashmap_iterator() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_hashmap_t map = allc_hashmap_str_new(allocator, sizeof(int));
    
    // Test iterator on empty map
    allc_hashmap_iterator_t iter = allc_hashmap_iterator_begin(&map);
    ALLC_TEST_BOOL(false, allc_hashmap_iterator_has_next(&iter));
    
    // Add entries
    int values[] = {10, 20, 30};
    allc_hashmap_str_put(&map, "ten", &values[0]);
    allc_hashmap_str_put(&map, "twenty", &values[1]);
    allc_hashmap_str_put(&map, "thirty", &values[2]);
    
    // Test iteration
    iter = allc_hashmap_iterator_begin(&map);
    int count = 0;
    bool found_ten = false, found_twenty = false, found_thirty = false;
    
    while (allc_hashmap_iterator_has_next(&iter)) {
        char *key = (char*)allc_hashmap_iterator_key(&iter);
        int *value = (int*)allc_hashmap_iterator_value(&iter);
        allc_size_t key_size = allc_hashmap_iterator_key_size(&iter);
        
        ALLC_TEST_BOOL(true, key != NULL);
        ALLC_TEST_BOOL(true, value != NULL);
        ALLC_TEST_BOOL(true, key_size > 0);
        
        if (strcmp(key, "ten") == 0) {
            found_ten = true;
            ALLC_TEST_ANY("10", "%d", *value);
        } else if (strcmp(key, "twenty") == 0) {
            found_twenty = true;
            ALLC_TEST_ANY("20", "%d", *value);
        } else if (strcmp(key, "thirty") == 0) {
            found_thirty = true;
            ALLC_TEST_ANY("30", "%d", *value);
        }
        
        count++;
        allc_hashmap_iterator_next(&iter);
    }
    
    ALLC_TEST_ANY("3", "%d", count);
    ALLC_TEST_BOOL(true, found_ten);
    ALLC_TEST_BOOL(true, found_twenty);
    ALLC_TEST_BOOL(true, found_thirty);
    
    allc_hashmap_delete(&map);
}

void test__allc_hashmap_foreach_macro() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_hashmap_t map = allc_hashmap_int_new(allocator, sizeof(double));
    
    // Add entries
    double values[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    for (int i = 0; i < 5; i++) {
        allc_hashmap_int_put(&map, i + 1, &values[i]);
    }
    
    // Test FOREACH macro
    int count = 0;
    double sum = 0.0;
    
    ALLC_HASHMAP_FOREACH(&map, iter) {
        allc_i64_t *key = (allc_i64_t*)allc_hashmap_iterator_key(&iter);
        double *value = (double*)allc_hashmap_iterator_value(&iter);
        
        ALLC_TEST_BOOL(true, key != NULL);
        ALLC_TEST_BOOL(true, value != NULL);
        ALLC_TEST_BOOL(true, *key >= 1 && *key <= 5);
        
        sum += *value;
        count++;
    }
    
    ALLC_TEST_ANY("5", "%d", count);
    ALLC_TEST_ANY("16.5", "%.1f", sum); // 1.1 + 2.2 + 3.3 + 4.4 + 5.5 = 16.5
    
    allc_hashmap_delete(&map);
}

// Hash Function Tests
// ===================

void test__allc_hashmap_hash_functions() {
    printf("\n[%s]\n", __func__);
    
    // Test string hash function
    allc_u64_t hash1 = allc_hashmap_hash_string("hello", 6);
    allc_u64_t hash2 = allc_hashmap_hash_string("hello", 6);
    allc_u64_t hash3 = allc_hashmap_hash_string("world", 6);
    
    ALLC_TEST_BOOL(true, hash1 == hash2); // Same string should have same hash
    ALLC_TEST_BOOL(true, hash1 != hash3); // Different strings should have different hashes
    ALLC_TEST_BOOL(true, hash1 != 0);     // Hash should not be zero
    
    // Test integer hash function
    allc_i64_t int_key1 = 42;
    allc_i64_t int_key2 = 42;
    allc_i64_t int_key3 = 100;
    
    allc_u64_t int_hash1 = allc_hashmap_hash_int(&int_key1, sizeof(int_key1));
    allc_u64_t int_hash2 = allc_hashmap_hash_int(&int_key2, sizeof(int_key2));
    allc_u64_t int_hash3 = allc_hashmap_hash_int(&int_key3, sizeof(int_key3));
    
    ALLC_TEST_BOOL(true, int_hash1 == int_hash2);
    ALLC_TEST_BOOL(true, int_hash1 != int_hash3);
    
    // Test bytes hash function
    char data1[] = {0x01, 0x02, 0x03, 0x04};
    char data2[] = {0x01, 0x02, 0x03, 0x04};
    char data3[] = {0x04, 0x03, 0x02, 0x01};
    
    allc_u64_t bytes_hash1 = allc_hashmap_hash_bytes(data1, sizeof(data1));
    allc_u64_t bytes_hash2 = allc_hashmap_hash_bytes(data2, sizeof(data2));
    allc_u64_t bytes_hash3 = allc_hashmap_hash_bytes(data3, sizeof(data3));
    
    ALLC_TEST_BOOL(true, bytes_hash1 == bytes_hash2);
    ALLC_TEST_BOOL(true, bytes_hash1 != bytes_hash3);
}

void test__allc_hashmap_equal_functions() {
    printf("\n[%s]\n", __func__);
    
    // Test string equality
    ALLC_TEST_BOOL(true, allc_hashmap_equal_string("hello", 6, "hello", 6));
    ALLC_TEST_BOOL(false, allc_hashmap_equal_string("hello", 6, "world", 6));
    ALLC_TEST_BOOL(false, allc_hashmap_equal_string("hello", 6, "hello", 5));
    
    // Test integer equality
    allc_i64_t int1 = 42, int2 = 42, int3 = 100;
    ALLC_TEST_BOOL(true, allc_hashmap_equal_int(&int1, sizeof(int1), &int2, sizeof(int2)));
    ALLC_TEST_BOOL(false, allc_hashmap_equal_int(&int1, sizeof(int1), &int3, sizeof(int3)));
    
    // Test bytes equality
    char data1[] = {0x01, 0x02, 0x03, 0x04};
    char data2[] = {0x01, 0x02, 0x03, 0x04};
    char data3[] = {0x04, 0x03, 0x02, 0x01};
    
    ALLC_TEST_BOOL(true, allc_hashmap_equal_bytes(data1, 4, data2, 4));
    ALLC_TEST_BOOL(false, allc_hashmap_equal_bytes(data1, 4, data3, 4));
    ALLC_TEST_BOOL(false, allc_hashmap_equal_bytes(data1, 4, data2, 3));
}

// Load Factor and Resizing Tests
// ===============================

void test__allc_hashmap_load_factor() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_hashmap_t map = allc_hashmap_str_new(allocator, sizeof(int));
    
    // Initial load factor should be 0
    ALLC_TEST_ANY("0.0", "%.1f", allc_hashmap_get_load_factor(&map));
    
    // Add some entries and check load factor increases
    int value = 42;
    for (int i = 0; i < 8; i++) {
        char key[32];
        sprintf(key, "key_%d", i);
        allc_hashmap_str_put(&map, key, &value);
    }
    
    allc_f32_t load_factor = allc_hashmap_get_load_factor(&map);
    ALLC_TEST_BOOL(true, load_factor > 0.0f);
    ALLC_TEST_BOOL(true, load_factor <= 1.0f);
    
    printf("Load factor with 8 entries: %.3f\n", load_factor);
    
    allc_hashmap_delete(&map);
}

void test__allc_hashmap_auto_resize() {
    printf("\n[%s]\n", __func__);
    bool success;
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_hashmap_t map = allc_hashmap_str_new(allocator, sizeof(int));
    
    allc_size_t initial_bucket_count = allc_hashmap_bucket_count(&map);
    printf("Initial bucket count: %zu\n", initial_bucket_count);
    
    // Add many entries to trigger resize
    int value = 1;
    for (int i = 0; i < 50; i++) {
        char key[32];
        sprintf(key, "key_%d", i);
        allc_hashmap_str_put(&map, key, &value);
    }
    
    allc_size_t final_bucket_count = allc_hashmap_bucket_count(&map);
    printf("Final bucket count: %zu\n", final_bucket_count);
    
    // Should have resized to accommodate entries
    ALLC_TEST_BOOL(true, final_bucket_count > initial_bucket_count);
    ALLC_TEST_ANY("50", "%zu", allc_hashmap_size(&map));
    
    // All entries should still be accessible
    success = true;
    for (int i = 0; success && i < 50; i++) {
        char key[32];
        sprintf(key, "key_%d", i);
        success &= allc_hashmap_str_has(&map, key);
    }
    ALLC_TEST_BOOL(true, success);
    
    allc_hashmap_delete(&map);
}

// Error Handling Tests
// ====================

void test__allc_hashmap_error_conditions() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    
    // Test operations on uninitialized map using generic functions
    allc_hashmap_t empty_map = {0};
    
    int value = 42;
    ALLC_TEST_ANY("1", "%d", allc_hashmap_put(&empty_map, "key", 4, &value));
    ALLC_TEST_BOOL(true, allc_hashmap_get(&empty_map, "key", 4) == NULL);
    ALLC_TEST_BOOL(false, allc_hashmap_has(&empty_map, "key", 4));
    ALLC_TEST_ANY("1", "%d", allc_hashmap_remove(&empty_map, "key", 4));
    
    // Test operations with NULL parameters on valid map
    allc_hashmap_t map = allc_hashmap_str_new(allocator, sizeof(int));
    
    ALLC_TEST_ANY("1", "%d", allc_hashmap_put(&map, NULL, 4, &value));
    ALLC_TEST_BOOL(true, allc_hashmap_get(&map, NULL, 4) == NULL);
    ALLC_TEST_BOOL(false, allc_hashmap_has(&map, NULL, 4));
    
    ALLC_TEST_ANY("1", "%d", allc_hashmap_put(&map, "key", 4, NULL));
    
    allc_hashmap_delete(&map);
    
    // Test invalid creation parameters
    allc_hashmap_t invalid_map = allc_hashmap_new(NULL, sizeof(int),
                                                 allc_hashmap_hash_string,
                                                 allc_hashmap_equal_string,
                                                 NULL, NULL);
    ALLC_TEST_BOOL(true, invalid_map.buckets == NULL);
    
    invalid_map = allc_hashmap_new(allocator, 0,
                                  allc_hashmap_hash_string,
                                  allc_hashmap_equal_string,
                                  NULL, NULL);
    ALLC_TEST_BOOL(true, invalid_map.buckets == NULL);
    
    invalid_map = allc_hashmap_new(allocator, sizeof(int),
                                  NULL, // NULL hash function
                                  allc_hashmap_equal_string,
                                  NULL, NULL);
    ALLC_TEST_BOOL(true, invalid_map.buckets == NULL);
}

// Custom Key Type Tests
// =====================

typedef struct {
    int x, y;
} point_t;

static allc_u64_t hash_point(const void *key, allc_size_t key_size) {
    (void)key_size;
    const point_t *p = (const point_t*)key;
    return (allc_u64_t)p->x * 31 + (allc_u64_t)p->y;
}

static allc_bool_t equal_point(const void *key1, allc_size_t key1_size,
                              const void *key2, allc_size_t key2_size) {
    (void)key1_size; (void)key2_size;
    const point_t *p1 = (const point_t*)key1;
    const point_t *p2 = (const point_t*)key2;
    return p1->x == p2->x && p1->y == p2->y;
}

void test__allc_hashmap_custom_keys() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_hashmap_t map = allc_hashmap_new(allocator, sizeof(char*),
                                         hash_point, equal_point,
                                         NULL, NULL);
    
    // Add entries with point keys
    point_t p1 = {10, 20};
    point_t p2 = {30, 40};
    point_t p3 = {10, 20}; // Same as p1
    
    char *value1 = "origin area";
    char *value2 = "far corner";
    
    ALLC_TEST_ANY("0", "%d", allc_hashmap_put(&map, &p1, sizeof(point_t), &value1));
    ALLC_TEST_ANY("0", "%d", allc_hashmap_put(&map, &p2, sizeof(point_t), &value2));
    
    ALLC_TEST_ANY("2", "%zu", allc_hashmap_size(&map));
    
    // Test get with equivalent point
    char **result1 = (char**)allc_hashmap_get(&map, &p3, sizeof(point_t));
    ALLC_TEST_BOOL(true, result1 != NULL);
    ALLC_TEST_BOOL(true, strcmp(*result1, "origin area") == 0);
    
    // Test get with different point
    char **result2 = (char**)allc_hashmap_get(&map, &p2, sizeof(point_t));
    ALLC_TEST_BOOL(true, result2 != NULL);
    ALLC_TEST_BOOL(true, strcmp(*result2, "far corner") == 0);
    
    // Test update with equivalent key
    char *new_value = "updated area";
    ALLC_TEST_ANY("0", "%d", allc_hashmap_put(&map, &p3, sizeof(point_t), &new_value));
    ALLC_TEST_ANY("2", "%zu", allc_hashmap_size(&map)); // Should not increase
    
    result1 = (char**)allc_hashmap_get(&map, &p1, sizeof(point_t));
    ALLC_TEST_BOOL(true, result1 != NULL);
    ALLC_TEST_BOOL(true, strcmp(*result1, "updated area") == 0);
    
    allc_hashmap_delete(&map);
}

// Performance and Stress Tests
// ============================

void test__allc_hashmap_stress() {
    printf("\n[%s]\n", __func__);
    bool success;
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_hashmap_t map = allc_hashmap_str_new(allocator, sizeof(int));
    
    const int num_entries = 1000;
    
    // Add many entries
    for (int i = 0; i < num_entries; i++) {
        char key[32];
        sprintf(key, "stress_key_%d", i);
        allc_hashmap_str_put(&map, key, &i);
    }
    
    ALLC_TEST_ANY("1000", "%zu", allc_hashmap_size(&map));
    
    // Verify all entries exist and have correct values
    success = true;
    for (int i = 0; success && i < num_entries; i++) {
        char key[32];
        sprintf(key, "stress_key_%d", i);
        
        if (!( success = allc_hashmap_str_has(&map, key) )) 
            break;
        
        int *value = (int*)allc_hashmap_str_get(&map, key);
        success &= (value != NULL);
        if (value) {
            success &= (*value == i);
        }
    }
    ALLC_TEST_BOOL(true, success);
    
    // Remove half the entries
    success = true;
    for (int i = 0; success && i < num_entries; i += 2) {
        char key[32];
        sprintf(key, "stress_key_%d", i);
        success &= allc_hashmap_str_remove(&map, key) == 0;
    }
    ALLC_TEST_BOOL(true, success);    
    ALLC_TEST_ANY("500", "%zu", allc_hashmap_size(&map));
    
    // Verify correct entries remain
    success = true;
    for (int i = 0; success && i < num_entries; i++) {
        char key[32];
        sprintf(key, "stress_key_%d", i);
        
        success &= (i % 2 == 0) == !allc_hashmap_str_has(&map, key);
    }
    ALLC_TEST_BOOL(true, success);
    
    allc_hashmap_delete(&map);
    
    printf("Stress test completed successfully\n");
}

void test__allc_hashmap_collision_handling() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    
    // Create a small map to force collisions
    allc_hashmap_t map = allc_hashmap_new_with_capacity(allocator, sizeof(int), 4, 10.0f, // High load factor
                                                       allc_hashmap_hash_string,
                                                       allc_hashmap_equal_string,
                                                       allc_hashmap_key_copy_string,
                                                       allc_hashmap_key_free_string);
    
    // Add many entries to a small map (forces collisions)
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    char keys[10][32];
    
    for (int i = 0; i < 10; i++) {
        sprintf(keys[i], "collision_key_%d", i);
        allc_hashmap_str_put(&map, keys[i], &values[i]);
    }
    
    ALLC_TEST_ANY("10", "%zu", allc_hashmap_size(&map));
    
    // Verify all entries are accessible despite collisions
    bool success = true;
    for (int i = 0; success && i < 10; i++) {
        int *value = (int*)allc_hashmap_str_get(&map, keys[i]);
        success &= (value != NULL);
        if (value) {
            success &= (*value == values[i]);
        }
    }
    ALLC_TEST_BOOL(true, success);
    
    allc_hashmap_delete(&map);
}

// Main test runner
int main() {
    printf("=== Hash Map Tests ===\n");
    
    // Basic functionality
    test__allc_hashmap_creation_deletion();
    test__allc_hashmap_with_capacity();
    test__allc_hashmap_str_operations();
    test__allc_hashmap_int_operations();
    test__allc_hashmap_remove_operations();
    test__allc_hashmap_clear();
    test__allc_hashmap_copy();
    
    // Iterator functionality
    test__allc_hashmap_iterator();
    test__allc_hashmap_foreach_macro();
    
    // Hash and equality functions
    test__allc_hashmap_hash_functions();
    test__allc_hashmap_equal_functions();
    
    // Load factor and resizing
    test__allc_hashmap_load_factor();
    test__allc_hashmap_auto_resize();
    
    // Error handling
    test__allc_hashmap_error_conditions();
    
    // Custom key types
    test__allc_hashmap_custom_keys();
    
    // Performance and stress tests
    test__allc_hashmap_stress();
    test__allc_hashmap_collision_handling();
    
    printf("\n=== All Hash Map Tests Completed ===\n");
    return 0;
}

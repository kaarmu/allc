/**
 * Hash Maps
 * =========
 *
 * Description
 * -----------
 * Generic hash map implementation with configurable hash functions.
 * String-to-value and generic key-to-value mappings.
 * Collision resolution through chaining.
 * Automatic resizing with load factor management.
 * Built on allocator abstraction.
 *
 * Features:
 * - Generic key-value storage with custom hash and equality functions
 * - String-optimized hash map variant for common use case
 * - Automatic resizing when load factor exceeds threshold
 * - Collision resolution via separate chaining
 * - Iterator support for traversal
 * - Memory management through allocator abstraction
 *
 * Usage Example:
 * ==============
 *
 * ```c
 * #define ALLC_IMPL
 * #include "hashmap.h"
 * 
 * int main() {
 *     allc_allocator_t allocator = allc_allocator_system();
 *     
 *     // String-to-integer map
 *     allc_hashmap_t map = allc_hashmap_str_new(allocator, sizeof(int));
 *     
 *     int value = 42;
 *     allc_hashmap_str_put(&map, "answer", &value);
 *     
 *     int *result = (int*)allc_hashmap_str_get(&map, "answer");
 *     if (result) {
 *         printf("The answer is %d\n", *result);
 *     }
 *     
 *     allc_hashmap_delete(&map);
 *     return 0;
 * }
 * ```
 *
 * Options
 * =======
 * #define ALLC_IMPL before including to get the implementation.
 *
 * (Authored by Kaj Munhoz Arfvidsson, 2023)
 **/

#ifndef ALLC_HASHMAP__GUARD
#define ALLC_HASHMAP__GUARD

// Includes {{{1
// =============

#include "allocator.h"
#include "cstr.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Macros {{{1
// ===========

#ifdef ALLC_IMPL
#define ALLC_HASHMAP_IMPL
#endif

// Error codes for hashmap operations
#define ALLC_HASHMAP_SUCCESS         0
#define ALLC_HASHMAP_ERROR_NULL      1
#define ALLC_HASHMAP_ERROR_NOMEM     2
#define ALLC_HASHMAP_ERROR_NOTFOUND  3
#define ALLC_HASHMAP_ERROR_EXISTS    4

// Default configuration
#define ALLC_HASHMAP_DEFAULT_CAPACITY    16
#define ALLC_HASHMAP_DEFAULT_LOAD_FACTOR 0.75f

// Type Definitions {{{1
// =====================

// Forward declarations
typedef struct allc_hashmap_s allc_hashmap_t;
typedef struct allc_hashmap_entry_s allc_hashmap_entry_t;
typedef struct allc_hashmap_iterator_s allc_hashmap_iterator_t;

// Hash function type - returns hash value for given key
typedef allc_u64_t (*allc_hashmap_hash_fn_t)(const void *key, allc_size_t key_size);

// Equality function type - returns true if keys are equal
typedef allc_bool_t (*allc_hashmap_equal_fn_t)(const void *key1, allc_size_t key1_size, 
                                               const void *key2, allc_size_t key2_size);

// Key copy function type - copies key data (optional, can be NULL for POD keys)
typedef void (*allc_hashmap_key_copy_fn_t)(void *dst, const void *src, allc_size_t size);

// Key free function type - frees key data (optional, can be NULL for POD keys)
typedef void (*allc_hashmap_key_free_fn_t)(void *key, allc_size_t size, allc_allocator_t allocator);

/**
 * Hash map entry - internal structure for chaining
 */
struct allc_hashmap_entry_s {
    void *key;                          // Key data
    allc_size_t key_size;               // Size of key in bytes
    void *value;                        // Value data
    allc_u64_t hash;                    // Cached hash value
    allc_hashmap_entry_t *next;         // Next entry in chain
};

/**
 * Hash map structure
 */
struct allc_hashmap_s {
    allc_hashmap_entry_t **buckets;     // Array of bucket heads
    allc_size_t bucket_count;           // Number of buckets
    allc_size_t size;                   // Number of entries
    allc_size_t value_size;             // Size of each value in bytes
    allc_f32_t load_factor;             // Maximum load factor before resize
    
    allc_hashmap_hash_fn_t hash_fn;     // Hash function
    allc_hashmap_equal_fn_t equal_fn;   // Key equality function
    allc_hashmap_key_copy_fn_t key_copy_fn;  // Key copy function (optional)
    allc_hashmap_key_free_fn_t key_free_fn;  // Key free function (optional)
    
    allc_allocator_t allocator;         // Memory allocator
};

/**
 * Hash map iterator
 */
struct allc_hashmap_iterator_s {
    allc_hashmap_t *map;                // Hash map being iterated
    allc_size_t bucket_index;           // Current bucket index
    allc_hashmap_entry_t *current;      // Current entry
};

// Function Declarations {{{1
// ==========================

/* Hash Map Creation and Destruction */

/**
 * Create a new hash map with custom hash and equality functions.
 * 
 * @param allocator Memory allocator to use
 * @param value_size Size of each value in bytes
 * @param hash_fn Hash function for keys
 * @param equal_fn Equality function for keys
 * @param key_copy_fn Key copy function (can be NULL for POD keys)
 * @param key_free_fn Key free function (can be NULL for POD keys)
 * @return New hash map
 */
allc_hashmap_t allc_hashmap_new(allc_allocator_t allocator, 
                               allc_size_t value_size,
                               allc_hashmap_hash_fn_t hash_fn,
                               allc_hashmap_equal_fn_t equal_fn,
                               allc_hashmap_key_copy_fn_t key_copy_fn,
                               allc_hashmap_key_free_fn_t key_free_fn);

/**
 * Create a new hash map with custom configuration.
 */
allc_hashmap_t allc_hashmap_new_with_capacity(allc_allocator_t allocator,
                                             allc_size_t value_size,
                                             allc_size_t initial_capacity,
                                             allc_f32_t load_factor,
                                             allc_hashmap_hash_fn_t hash_fn,
                                             allc_hashmap_equal_fn_t equal_fn,
                                             allc_hashmap_key_copy_fn_t key_copy_fn,
                                             allc_hashmap_key_free_fn_t key_free_fn);

/**
 * Create a new string-keyed hash map (convenience function).
 */
allc_hashmap_t allc_hashmap_str_new(allc_allocator_t allocator, allc_size_t value_size);

/**
 * Create a new integer-keyed hash map (convenience function).
 */
allc_hashmap_t allc_hashmap_int_new(allc_allocator_t allocator, allc_size_t value_size);

/**
 * Create a copy of an existing hash map.
 */
allc_hashmap_t allc_hashmap_copy(const allc_hashmap_t *self);

/**
 * Delete the hash map and free all associated memory.
 */
void allc_hashmap_delete(allc_hashmap_t *self);

/* Hash Map Operations */

/**
 * Insert or update a key-value pair in the hash map.
 * 
 * @param self Hash map to modify
 * @param key Pointer to key data
 * @param key_size Size of key in bytes
 * @param value Pointer to value data
 * @return ALLC_HASHMAP_SUCCESS on success, error code on failure
 */
allc_i32_t allc_hashmap_put(allc_hashmap_t *self, const void *key, allc_size_t key_size, const void *value);

/**
 * Get a value from the hash map by key.
 * 
 * @param self Hash map to search
 * @param key Pointer to key data
 * @param key_size Size of key in bytes
 * @return Pointer to value data, or NULL if not found
 */
void *allc_hashmap_get(const allc_hashmap_t *self, const void *key, allc_size_t key_size);

/**
 * Check if a key exists in the hash map.
 */
allc_bool_t allc_hashmap_has(const allc_hashmap_t *self, const void *key, allc_size_t key_size);

/**
 * Remove a key-value pair from the hash map.
 * 
 * @param self Hash map to modify
 * @param key Pointer to key data
 * @param key_size Size of key in bytes
 * @return ALLC_HASHMAP_SUCCESS if removed, ALLC_HASHMAP_ERROR_NOTFOUND if key not found
 */
allc_i32_t allc_hashmap_remove(allc_hashmap_t *self, const void *key, allc_size_t key_size);

/**
 * Clear all entries from the hash map.
 */
void allc_hashmap_clear(allc_hashmap_t *self);

/* String-Keyed Convenience Functions */

/**
 * Insert or update a string key-value pair.
 */
allc_i32_t allc_hashmap_str_put(allc_hashmap_t *self, allc_string_t key, const void *value);

/**
 * Get a value by string key.
 */
void *allc_hashmap_str_get(const allc_hashmap_t *self, allc_string_t key);

/**
 * Check if string key exists.
 */
allc_bool_t allc_hashmap_str_has(const allc_hashmap_t *self, allc_string_t key);

/**
 * Remove by string key.
 */
allc_i32_t allc_hashmap_str_remove(allc_hashmap_t *self, allc_string_t key);

/* Integer-Keyed Convenience Functions */

/**
 * Insert or update an integer key-value pair.
 */
allc_i32_t allc_hashmap_int_put(allc_hashmap_t *self, allc_i64_t key, const void *value);

/**
 * Get a value by integer key.
 */
void *allc_hashmap_int_get(const allc_hashmap_t *self, allc_i64_t key);

/**
 * Check if integer key exists.
 */
allc_bool_t allc_hashmap_int_has(const allc_hashmap_t *self, allc_i64_t key);

/**
 * Remove by integer key.
 */
allc_i32_t allc_hashmap_int_remove(allc_hashmap_t *self, allc_i64_t key);

/* Hash Map Information */

/**
 * Get the number of entries in the hash map.
 */
allc_size_t allc_hashmap_size(const allc_hashmap_t *self);

/**
 * Check if the hash map is empty.
 */
allc_bool_t allc_hashmap_is_empty(const allc_hashmap_t *self);

/**
 * Get the current load factor of the hash map.
 */
allc_f32_t allc_hashmap_get_load_factor(const allc_hashmap_t *self);

/**
 * Get the number of buckets in the hash map.
 */
allc_size_t allc_hashmap_bucket_count(const allc_hashmap_t *self);

/* Hash Map Iterator */

/**
 * Initialize an iterator for the hash map.
 */
allc_hashmap_iterator_t allc_hashmap_iterator_begin(allc_hashmap_t *self);

/**
 * Check if the iterator has a current entry.
 */
allc_bool_t allc_hashmap_iterator_has_next(const allc_hashmap_iterator_t *iter);

/**
 * Get the current entry's key.
 */
void *allc_hashmap_iterator_key(const allc_hashmap_iterator_t *iter);

/**
 * Get the current entry's key size.
 */
allc_size_t allc_hashmap_iterator_key_size(const allc_hashmap_iterator_t *iter);

/**
 * Get the current entry's value.
 */
void *allc_hashmap_iterator_value(const allc_hashmap_iterator_t *iter);

/**
 * Advance the iterator to the next entry.
 */
void allc_hashmap_iterator_next(allc_hashmap_iterator_t *iter);

/* Built-in Hash Functions */

/**
 * String hash function (FNV-1a algorithm).
 */
allc_u64_t allc_hashmap_hash_string(const void *key, allc_size_t key_size);

/**
 * Integer hash function.
 */
allc_u64_t allc_hashmap_hash_int(const void *key, allc_size_t key_size);

/**
 * Generic hash function for binary data.
 */
allc_u64_t allc_hashmap_hash_bytes(const void *key, allc_size_t key_size);

/* Built-in Equality Functions */

/**
 * String equality function.
 */
allc_bool_t allc_hashmap_equal_string(const void *key1, allc_size_t key1_size,
                                     const void *key2, allc_size_t key2_size);

/**
 * Integer equality function.
 */
allc_bool_t allc_hashmap_equal_int(const void *key1, allc_size_t key1_size,
                                  const void *key2, allc_size_t key2_size);

/**
 * Generic byte-wise equality function.
 */
allc_bool_t allc_hashmap_equal_bytes(const void *key1, allc_size_t key1_size,
                                    const void *key2, allc_size_t key2_size);

/* Built-in Key Management Functions */

/**
 * String key copy function.
 */
void allc_hashmap_key_copy_string(void *dst, const void *src, allc_size_t size);

/**
 * String key free function.
 */
void allc_hashmap_key_free_string(void *key, allc_size_t size, allc_allocator_t allocator);

/* Macro Helpers */

/**
 * Iterate over all entries in a hash map.
 * Usage: ALLC_HASHMAP_FOREACH(map, iter) { ... }
 */
#define ALLC_HASHMAP_FOREACH(map, iter) \
    for (allc_hashmap_iterator_t iter = allc_hashmap_iterator_begin(map); \
         allc_hashmap_iterator_has_next(&iter); \
         allc_hashmap_iterator_next(&iter))

#endif // ALLC_HASHMAP__GUARD

/****************************************************************************/

#ifndef ALLC_HASHMAP_IMPL__GUARD
#define ALLC_HASHMAP_IMPL__GUARD
#else
#undef ALLC_HASHMAP_IMPL
#endif

#ifdef ALLC_HASHMAP_IMPL

// Includes {{{1
// =============

#include <string.h>
#include <math.h>

// Internal Helper Functions {{{1
// ==============================

/**
 * Get the next power of 2 greater than or equal to x.
 */
static allc_size_t allc_hashmap_next_power_of_2(allc_size_t x) {
    if (x <= 1) return 2;
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    if (sizeof(allc_size_t) > 4) {
        x |= x >> 32;
    }
    return x + 1;
}

/**
 * Create a new entry.
 */
static allc_hashmap_entry_t *allc_hashmap_entry_new(allc_allocator_t allocator,
                                                    const void *key, allc_size_t key_size,
                                                    const void *value, allc_size_t value_size,
                                                    allc_u64_t hash,
                                                    allc_hashmap_key_copy_fn_t key_copy_fn) {
    allc_hashmap_entry_t *entry = (allc_hashmap_entry_t*)allc_allocator_alloc(allocator, sizeof(allc_hashmap_entry_t));
    if (!entry) return NULL;
    
    // Allocate key storage
    entry->key = allc_allocator_alloc(allocator, key_size);
    if (!entry->key) {
        allc_allocator_free(allocator, entry);
        return NULL;
    }
    
    // Allocate value storage
    entry->value = allc_allocator_alloc(allocator, value_size);
    if (!entry->value) {
        allc_allocator_free(allocator, entry->key);
        allc_allocator_free(allocator, entry);
        return NULL;
    }
    
    // Copy key data
    if (key_copy_fn) {
        key_copy_fn(entry->key, key, key_size);
    } else {
        memcpy(entry->key, key, key_size);
    }
    
    // Copy value data
    memcpy(entry->value, value, value_size);
    
    entry->key_size = key_size;
    entry->hash = hash;
    entry->next = NULL;
    
    return entry;
}

/**
 * Delete an entry and free its memory.
 */
static void allc_hashmap_entry_delete(allc_hashmap_entry_t *entry, 
                                      allc_allocator_t allocator,
                                      allc_hashmap_key_free_fn_t key_free_fn) {
    if (!entry) return;
    
    if (key_free_fn) {
        key_free_fn(entry->key, entry->key_size, allocator);
    }
    
    allc_allocator_free(allocator, entry->key);
    allc_allocator_free(allocator, entry->value);
    allc_allocator_free(allocator, entry);
}

/**
 * Resize the hash map to a new bucket count.
 */
static allc_i32_t allc_hashmap_resize(allc_hashmap_t *self, allc_size_t new_bucket_count) {
    if (!self || new_bucket_count == 0) return ALLC_HASHMAP_ERROR_NULL;
    
    // Allocate new bucket array
    allc_hashmap_entry_t **new_buckets = (allc_hashmap_entry_t**)allc_allocator_alloc(
        self->allocator, sizeof(allc_hashmap_entry_t*) * new_bucket_count);
    if (!new_buckets) return ALLC_HASHMAP_ERROR_NOMEM;
    
    // Initialize new buckets to NULL
    memset(new_buckets, 0, sizeof(allc_hashmap_entry_t*) * new_bucket_count);
    
    // Save old buckets
    allc_hashmap_entry_t **old_buckets = self->buckets;
    allc_size_t old_bucket_count = self->bucket_count;
    
    // Update map structure
    self->buckets = new_buckets;
    self->bucket_count = new_bucket_count;
    
    // Rehash all entries
    for (allc_size_t i = 0; i < old_bucket_count; i++) {
        allc_hashmap_entry_t *entry = old_buckets[i];
        while (entry) {
            allc_hashmap_entry_t *next = entry->next;
            
            // Calculate new bucket index
            allc_size_t bucket_index = entry->hash % new_bucket_count;
            
            // Insert at head of new bucket
            entry->next = new_buckets[bucket_index];
            new_buckets[bucket_index] = entry;
            
            entry = next;
        }
    }
    
    // Free old bucket array
    allc_allocator_free(self->allocator, old_buckets);
    
    return ALLC_HASHMAP_SUCCESS;
}

/**
 * Check if the map needs to be resized and resize if necessary.
 */
static allc_i32_t allc_hashmap_maybe_resize(allc_hashmap_t *self) {
    if (!self) return ALLC_HASHMAP_ERROR_NULL;
    
    allc_f32_t current_load = (allc_f32_t)self->size / (allc_f32_t)self->bucket_count;
    if (current_load > self->load_factor) {
        return allc_hashmap_resize(self, self->bucket_count * 2);
    }
    
    return ALLC_HASHMAP_SUCCESS;
}

// Public API Implementation {{{1
// ==============================

allc_hashmap_t allc_hashmap_new(allc_allocator_t allocator, 
                               allc_size_t value_size,
                               allc_hashmap_hash_fn_t hash_fn,
                               allc_hashmap_equal_fn_t equal_fn,
                               allc_hashmap_key_copy_fn_t key_copy_fn,
                               allc_hashmap_key_free_fn_t key_free_fn) {
    return allc_hashmap_new_with_capacity(allocator, value_size,
                                         ALLC_HASHMAP_DEFAULT_CAPACITY,
                                         ALLC_HASHMAP_DEFAULT_LOAD_FACTOR,
                                         hash_fn, equal_fn, key_copy_fn, key_free_fn);
}

allc_hashmap_t allc_hashmap_new_with_capacity(allc_allocator_t allocator,
                                             allc_size_t value_size,
                                             allc_size_t initial_capacity,
                                             allc_f32_t load_factor,
                                             allc_hashmap_hash_fn_t hash_fn,
                                             allc_hashmap_equal_fn_t equal_fn,
                                             allc_hashmap_key_copy_fn_t key_copy_fn,
                                             allc_hashmap_key_free_fn_t key_free_fn) {
    allc_hashmap_t map = {0};
    
    if (!allocator || !hash_fn || !equal_fn || value_size == 0) {
        return map; // Return empty map on invalid input
    }
    
    // Round up to next power of 2
    initial_capacity = allc_hashmap_next_power_of_2(initial_capacity);
    
    // Allocate bucket array
    map.buckets = (allc_hashmap_entry_t**)allc_allocator_alloc(
        allocator, sizeof(allc_hashmap_entry_t*) * initial_capacity);
    if (!map.buckets) {
        return map; // Return empty map on allocation failure
    }
    
    // Initialize bucket array
    memset(map.buckets, 0, sizeof(allc_hashmap_entry_t*) * initial_capacity);
    
    // Initialize map fields
    map.bucket_count = initial_capacity;
    map.size = 0;
    map.value_size = value_size;
    map.load_factor = load_factor;
    map.hash_fn = hash_fn;
    map.equal_fn = equal_fn;
    map.key_copy_fn = key_copy_fn;
    map.key_free_fn = key_free_fn;
    map.allocator = allocator;
    
    return map;
}

allc_hashmap_t allc_hashmap_str_new(allc_allocator_t allocator, allc_size_t value_size) {
    return allc_hashmap_new(allocator, value_size,
                           allc_hashmap_hash_string,
                           allc_hashmap_equal_string,
                           allc_hashmap_key_copy_string,
                           allc_hashmap_key_free_string);
}

allc_hashmap_t allc_hashmap_int_new(allc_allocator_t allocator, allc_size_t value_size) {
    return allc_hashmap_new(allocator, value_size,
                           allc_hashmap_hash_int,
                           allc_hashmap_equal_int,
                           NULL, // No special key copy needed for integers
                           NULL); // No special key free needed for integers
}

allc_hashmap_t allc_hashmap_copy(const allc_hashmap_t *self) {
    allc_hashmap_t copy = {0};
    
    if (!self || !self->buckets) return copy;
    
    // Create new map with same configuration
    copy = allc_hashmap_new_with_capacity(self->allocator, self->value_size,
                                         self->bucket_count, self->load_factor,
                                         self->hash_fn, self->equal_fn,
                                         self->key_copy_fn, self->key_free_fn);
    if (!copy.buckets) return copy;
    
    // Copy all entries
    for (allc_size_t i = 0; i < self->bucket_count; i++) {
        allc_hashmap_entry_t *entry = self->buckets[i];
        while (entry) {
            allc_hashmap_put(&copy, entry->key, entry->key_size, entry->value);
            entry = entry->next;
        }
    }
    
    return copy;
}

void allc_hashmap_delete(allc_hashmap_t *self) {
    if (!self || !self->buckets) return;
    
    // Free all entries
    for (allc_size_t i = 0; i < self->bucket_count; i++) {
        allc_hashmap_entry_t *entry = self->buckets[i];
        while (entry) {
            allc_hashmap_entry_t *next = entry->next;
            allc_hashmap_entry_delete(entry, self->allocator, self->key_free_fn);
            entry = next;
        }
    }
    
    // Free bucket array
    allc_allocator_free(self->allocator, self->buckets);
    
    // Clear map structure
    memset(self, 0, sizeof(allc_hashmap_t));
}

allc_i32_t allc_hashmap_put(allc_hashmap_t *self, const void *key, allc_size_t key_size, const void *value) {
    if (!self || !self->buckets || !key || !value || key_size == 0) {
        return ALLC_HASHMAP_ERROR_NULL;
    }
    
    // Calculate hash
    allc_u64_t hash = self->hash_fn(key, key_size);
    allc_size_t bucket_index = hash % self->bucket_count;
    
    // Search for existing entry
    allc_hashmap_entry_t *entry = self->buckets[bucket_index];
    while (entry) {
        if (entry->hash == hash && entry->key_size == key_size &&
            self->equal_fn(entry->key, entry->key_size, key, key_size)) {
            // Update existing entry
            memcpy(entry->value, value, self->value_size);
            return ALLC_HASHMAP_SUCCESS;
        }
        entry = entry->next;
    }
    
    // Create new entry
    entry = allc_hashmap_entry_new(self->allocator, key, key_size, value, self->value_size,
                                  hash, self->key_copy_fn);
    if (!entry) return ALLC_HASHMAP_ERROR_NOMEM;
    
    // Insert at head of bucket
    entry->next = self->buckets[bucket_index];
    self->buckets[bucket_index] = entry;
    self->size++;
    
    // Check if resize is needed
    return allc_hashmap_maybe_resize(self);
}

void *allc_hashmap_get(const allc_hashmap_t *self, const void *key, allc_size_t key_size) {
    if (!self || !self->buckets || !key || key_size == 0) return NULL;
    
    // Calculate hash
    allc_u64_t hash = self->hash_fn(key, key_size);
    allc_size_t bucket_index = hash % self->bucket_count;
    
    // Search for entry
    allc_hashmap_entry_t *entry = self->buckets[bucket_index];
    while (entry) {
        if (entry->hash == hash && entry->key_size == key_size &&
            self->equal_fn(entry->key, entry->key_size, key, key_size)) {
            return entry->value;
        }
        entry = entry->next;
    }
    
    return NULL;
}

allc_bool_t allc_hashmap_has(const allc_hashmap_t *self, const void *key, allc_size_t key_size) {
    return allc_hashmap_get(self, key, key_size) != NULL;
}

allc_i32_t allc_hashmap_remove(allc_hashmap_t *self, const void *key, allc_size_t key_size) {
    if (!self || !self->buckets || !key || key_size == 0) {
        return ALLC_HASHMAP_ERROR_NULL;
    }
    
    // Calculate hash
    allc_u64_t hash = self->hash_fn(key, key_size);
    allc_size_t bucket_index = hash % self->bucket_count;
    
    // Search for entry
    allc_hashmap_entry_t **entry_ptr = &self->buckets[bucket_index];
    while (*entry_ptr) {
        allc_hashmap_entry_t *entry = *entry_ptr;
        if (entry->hash == hash && entry->key_size == key_size &&
            self->equal_fn(entry->key, entry->key_size, key, key_size)) {
            // Remove from chain
            *entry_ptr = entry->next;
            allc_hashmap_entry_delete(entry, self->allocator, self->key_free_fn);
            self->size--;
            return ALLC_HASHMAP_SUCCESS;
        }
        entry_ptr = &entry->next;
    }
    
    return ALLC_HASHMAP_ERROR_NOTFOUND;
}

void allc_hashmap_clear(allc_hashmap_t *self) {
    if (!self || !self->buckets) return;
    
    // Free all entries
    for (allc_size_t i = 0; i < self->bucket_count; i++) {
        allc_hashmap_entry_t *entry = self->buckets[i];
        while (entry) {
            allc_hashmap_entry_t *next = entry->next;
            allc_hashmap_entry_delete(entry, self->allocator, self->key_free_fn);
            entry = next;
        }
        self->buckets[i] = NULL;
    }
    
    self->size = 0;
}

// String convenience functions
allc_i32_t allc_hashmap_str_put(allc_hashmap_t *self, allc_string_t key, const void *value) {
    return allc_hashmap_put(self, key, strlen(key) + 1, value);
}

void *allc_hashmap_str_get(const allc_hashmap_t *self, allc_string_t key) {
    return allc_hashmap_get(self, key, strlen(key) + 1);
}

allc_bool_t allc_hashmap_str_has(const allc_hashmap_t *self, allc_string_t key) {
    return allc_hashmap_has(self, key, strlen(key) + 1);
}

allc_i32_t allc_hashmap_str_remove(allc_hashmap_t *self, allc_string_t key) {
    return allc_hashmap_remove(self, key, strlen(key) + 1);
}

// Integer convenience functions
allc_i32_t allc_hashmap_int_put(allc_hashmap_t *self, allc_i64_t key, const void *value) {
    return allc_hashmap_put(self, &key, sizeof(key), value);
}

void *allc_hashmap_int_get(const allc_hashmap_t *self, allc_i64_t key) {
    return allc_hashmap_get(self, &key, sizeof(key));
}

allc_bool_t allc_hashmap_int_has(const allc_hashmap_t *self, allc_i64_t key) {
    return allc_hashmap_has(self, &key, sizeof(key));
}

allc_i32_t allc_hashmap_int_remove(allc_hashmap_t *self, allc_i64_t key) {
    return allc_hashmap_remove(self, &key, sizeof(key));
}

// Information functions
allc_size_t allc_hashmap_size(const allc_hashmap_t *self) {
    return self ? self->size : 0;
}

allc_bool_t allc_hashmap_is_empty(const allc_hashmap_t *self) {
    return allc_hashmap_size(self) == 0;
}

allc_f32_t allc_hashmap_get_load_factor(const allc_hashmap_t *self) {
    if (!self || self->bucket_count == 0) return 0.0f;
    return (allc_f32_t)self->size / (allc_f32_t)self->bucket_count;
}

allc_size_t allc_hashmap_bucket_count(const allc_hashmap_t *self) {
    return self ? self->bucket_count : 0;
}

// Iterator functions
allc_hashmap_iterator_t allc_hashmap_iterator_begin(allc_hashmap_t *self) {
    allc_hashmap_iterator_t iter = {0};
    
    if (!self || !self->buckets) return iter;
    
    iter.map = self;
    iter.bucket_index = 0;
    iter.current = NULL;
    
    // Find first non-empty bucket
    for (allc_size_t i = 0; i < self->bucket_count; i++) {
        if (self->buckets[i]) {
            iter.bucket_index = i;
            iter.current = self->buckets[i];
            break;
        }
    }
    
    return iter;
}

allc_bool_t allc_hashmap_iterator_has_next(const allc_hashmap_iterator_t *iter) {
    return iter && iter->current != NULL;
}

void *allc_hashmap_iterator_key(const allc_hashmap_iterator_t *iter) {
    return iter && iter->current ? iter->current->key : NULL;
}

allc_size_t allc_hashmap_iterator_key_size(const allc_hashmap_iterator_t *iter) {
    return iter && iter->current ? iter->current->key_size : 0;
}

void *allc_hashmap_iterator_value(const allc_hashmap_iterator_t *iter) {
    return iter && iter->current ? iter->current->value : NULL;
}

void allc_hashmap_iterator_next(allc_hashmap_iterator_t *iter) {
    if (!iter || !iter->current || !iter->map) return;
    
    // Try next entry in current bucket
    iter->current = iter->current->next;
    
    // If no more entries in current bucket, find next non-empty bucket
    if (!iter->current) {
        for (allc_size_t i = iter->bucket_index + 1; i < iter->map->bucket_count; i++) {
            if (iter->map->buckets[i]) {
                iter->bucket_index = i;
                iter->current = iter->map->buckets[i];
                return;
            }
        }
        // No more entries
        iter->current = NULL;
    }
}

// Built-in hash functions
allc_u64_t allc_hashmap_hash_string(const void *key, allc_size_t key_size) {
    // FNV-1a hash algorithm
    const allc_u8_t *data = (const allc_u8_t*)key;
    allc_u64_t hash = 14695981039346656037ULL; // FNV offset basis
    
    for (allc_size_t i = 0; i < key_size - 1; i++) { // -1 to skip null terminator
        hash ^= data[i];
        hash *= 1099511628211ULL; // FNV prime
    }
    
    return hash;
}

allc_u64_t allc_hashmap_hash_int(const void *key, allc_size_t key_size) {
    (void)key_size; // Unused parameter
    
    allc_u64_t x = *(const allc_i64_t*)key;
    // Thomas Wang's 64-bit integer hash
    x = (~x) + (x << 21);
    x = x ^ (x >> 24);
    x = (x + (x << 3)) + (x << 8);
    x = x ^ (x >> 14);
    x = (x + (x << 2)) + (x << 4);
    x = x ^ (x >> 28);
    x = x + (x << 31);
    
    return x;
}

allc_u64_t allc_hashmap_hash_bytes(const void *key, allc_size_t key_size) {
    // FNV-1a hash for arbitrary bytes
    const allc_u8_t *data = (const allc_u8_t*)key;
    allc_u64_t hash = 14695981039346656037ULL; // FNV offset basis
    
    for (allc_size_t i = 0; i < key_size; i++) {
        hash ^= data[i];
        hash *= 1099511628211ULL; // FNV prime
    }
    
    return hash;
}

// Built-in equality functions
allc_bool_t allc_hashmap_equal_string(const void *key1, allc_size_t key1_size,
                                     const void *key2, allc_size_t key2_size) {
    if (key1_size != key2_size) return false;
    return strcmp((const char*)key1, (const char*)key2) == 0;
}

allc_bool_t allc_hashmap_equal_int(const void *key1, allc_size_t key1_size,
                                  const void *key2, allc_size_t key2_size) {
    (void)key1_size; (void)key2_size; // Unused parameters
    return *(const allc_i64_t*)key1 == *(const allc_i64_t*)key2;
}

allc_bool_t allc_hashmap_equal_bytes(const void *key1, allc_size_t key1_size,
                                    const void *key2, allc_size_t key2_size) {
    if (key1_size != key2_size) return false;
    return memcmp(key1, key2, key1_size) == 0;
}

// Built-in key management functions
void allc_hashmap_key_copy_string(void *dst, const void *src, allc_size_t size) {
    memcpy(dst, src, size);
}

void allc_hashmap_key_free_string(void *key, allc_size_t size, allc_allocator_t allocator) {
    (void)key; (void)size; (void)allocator; // No special cleanup needed for string keys
}

#endif // ALLC_HASHMAP_IMPL

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker

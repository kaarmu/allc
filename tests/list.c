#define ALLC_IMPL

#include "../dev/macro.h"
#include "../dev/list.h"

void test__allc_list_new_delete()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    // Test: New list should be empty
    ALLC_TEST_ANY("0", "%zu", list.length);
    
    allc_list_delete(&list);
    
    // Test: After delete, list should still have length 0
    ALLC_TEST_ANY("0", "%zu", list.length);
}

void test__allc_list_append()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    // Test: Append integers
    int values[] = {10, 20, 30};
    allc_list_append(&list, sizeof(int), &values[0]);
    allc_list_append(&list, sizeof(int), &values[1]);
    allc_list_append(&list, sizeof(int), &values[2]);
    
    ALLC_TEST_ANY("3", "%zu", list.length);
    
    // Test: Check values
    int *val0 = (int*)allc_list_at(&list, 0);
    int *val1 = (int*)allc_list_at(&list, 1);
    int *val2 = (int*)allc_list_at(&list, 2);
    
    ALLC_TEST_ANY("10", "%d", *val0);
    ALLC_TEST_ANY("20", "%d", *val1);
    ALLC_TEST_ANY("30", "%d", *val2);
    
    allc_list_delete(&list);
}

void test__allc_list_insert()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    // Setup initial list: [10, 20, 30]
    int values[] = {10, 20, 30, 15, 25};
    allc_list_append(&list, sizeof(int), &values[0]);
    allc_list_append(&list, sizeof(int), &values[1]);
    allc_list_append(&list, sizeof(int), &values[2]);
    
    // Test: Insert at beginning
    allc_list_insert(&list, 0, sizeof(int), &values[4]); // Insert 25 at position 0
    ALLC_TEST_ANY("4", "%zu", list.length);
    ALLC_TEST_ANY("25", "%d", *(int*)allc_list_at(&list, 0));
    ALLC_TEST_ANY("10", "%d", *(int*)allc_list_at(&list, 1));
    
    // Test: Insert in middle
    allc_list_insert(&list, 2, sizeof(int), &values[3]); // Insert 15 at position 2
    ALLC_TEST_ANY("5", "%zu", list.length);
    ALLC_TEST_ANY("25", "%d", *(int*)allc_list_at(&list, 0));
    ALLC_TEST_ANY("10", "%d", *(int*)allc_list_at(&list, 1));
    ALLC_TEST_ANY("15", "%d", *(int*)allc_list_at(&list, 2));
    ALLC_TEST_ANY("20", "%d", *(int*)allc_list_at(&list, 3));
    ALLC_TEST_ANY("30", "%d", *(int*)allc_list_at(&list, 4));
    
    // Test: Insert at end (should append)
    int end_val = 99;
    allc_list_insert(&list, list.length, sizeof(int), &end_val);
    ALLC_TEST_ANY("6", "%zu", list.length);
    ALLC_TEST_ANY("99", "%d", *(int*)allc_list_at(&list, 5));
    
    // Test: Insert beyond end (should append)
    int beyond_val = 88;
    allc_list_insert(&list, 100, sizeof(int), &beyond_val);
    ALLC_TEST_ANY("7", "%zu", list.length);
    ALLC_TEST_ANY("88", "%d", *(int*)allc_list_at(&list, 6));
    
    // Test: Insert with negative index beyond start (should insert at beginning)
    int neg_val = 77;
    allc_list_insert(&list, -100, sizeof(int), &neg_val);
    ALLC_TEST_ANY("8", "%zu", list.length);
    ALLC_TEST_ANY("77", "%d", *(int*)allc_list_at(&list, 0));
    
    allc_list_delete(&list);
}

void test__allc_list_at_negative_indices()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    // Setup list: [10, 20, 30]
    int values[] = {10, 20, 30};
    allc_list_append(&list, sizeof(int), &values[0]);
    allc_list_append(&list, sizeof(int), &values[1]);
    allc_list_append(&list, sizeof(int), &values[2]);
    
    // Test: Negative indices
    ALLC_TEST_ANY("30", "%d", *(int*)allc_list_at(&list, -1)); // Last element
    ALLC_TEST_ANY("20", "%d", *(int*)allc_list_at(&list, -2)); // Second to last
    ALLC_TEST_ANY("10", "%d", *(int*)allc_list_at(&list, -3)); // First element
    
    // Test: Out of bounds
    void *null_ptr = allc_list_at(&list, 10);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(null_ptr == NULL));
    
    null_ptr = allc_list_at(&list, -10);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(null_ptr == NULL));
    
    allc_list_delete(&list);
}

void test__allc_list_remove()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    // Setup list: [10, 20, 30, 40]
    int values[] = {10, 20, 30, 40};
    for (int i = 0; i < 4; i++) {
        allc_list_append(&list, sizeof(int), &values[i]);
    }
    
    // Test: Remove from middle
    allc_list_remove(&list, 1); // Remove 20
    ALLC_TEST_ANY("3", "%zu", list.length);
    ALLC_TEST_ANY("10", "%d", *(int*)allc_list_at(&list, 0));
    ALLC_TEST_ANY("30", "%d", *(int*)allc_list_at(&list, 1));
    ALLC_TEST_ANY("40", "%d", *(int*)allc_list_at(&list, 2));
    
    // Test: Remove from end
    allc_list_remove(&list, -1); // Remove 40
    ALLC_TEST_ANY("2", "%zu", list.length);
    ALLC_TEST_ANY("10", "%d", *(int*)allc_list_at(&list, 0));
    ALLC_TEST_ANY("30", "%d", *(int*)allc_list_at(&list, 1));
    
    // Test: Remove from beginning
    allc_list_remove(&list, 0); // Remove 10
    ALLC_TEST_ANY("1", "%zu", list.length);
    ALLC_TEST_ANY("30", "%d", *(int*)allc_list_at(&list, 0));
    
    allc_list_delete(&list);
}

void test__allc_list_reverse()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    // Setup list: [1, 2, 3, 4]
    int values[] = {1, 2, 3, 4};
    for (int i = 0; i < 4; i++) {
        allc_list_append(&list, sizeof(int), &values[i]);
    }
    
    // Test: Reverse
    allc_list_reverse(&list);
    ALLC_TEST_ANY("4", "%zu", list.length);
    ALLC_TEST_ANY("4", "%d", *(int*)allc_list_at(&list, 0));
    ALLC_TEST_ANY("3", "%d", *(int*)allc_list_at(&list, 1));
    ALLC_TEST_ANY("2", "%d", *(int*)allc_list_at(&list, 2));
    ALLC_TEST_ANY("1", "%d", *(int*)allc_list_at(&list, 3));
    
    allc_list_delete(&list);
}

void test__allc_list_extend()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list1 = allc_list_new(allocator);
    allc_list_t list2 = allc_list_new(allocator);
    
    // Setup list1: [10, 20]
    int values1[] = {10, 20};
    allc_list_append(&list1, sizeof(int), &values1[0]);
    allc_list_append(&list1, sizeof(int), &values1[1]);
    
    // Setup list2: [30, 40]
    int values2[] = {30, 40};
    allc_list_append(&list2, sizeof(int), &values2[0]);
    allc_list_append(&list2, sizeof(int), &values2[1]);
    
    // Test: Extend list1 with list2
    allc_list_extend(&list1, &list2);
    ALLC_TEST_ANY("4", "%zu", list1.length);
    ALLC_TEST_ANY("10", "%d", *(int*)allc_list_at(&list1, 0));
    ALLC_TEST_ANY("20", "%d", *(int*)allc_list_at(&list1, 1));
    ALLC_TEST_ANY("30", "%d", *(int*)allc_list_at(&list1, 2));
    ALLC_TEST_ANY("40", "%d", *(int*)allc_list_at(&list1, 3));
    
    // Test: list2 should remain unchanged
    ALLC_TEST_ANY("2", "%zu", list2.length);
    
    allc_list_delete(&list1);
    allc_list_delete(&list2);
}

void test__allc_list_swap()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    // Setup list: [10, 20, 30, 40]
    int values[] = {10, 20, 30, 40};
    for (int i = 0; i < 4; i++) {
        allc_list_append(&list, sizeof(int), &values[i]);
    }
    
    // Test: Swap elements at indices 1 and 3
    allc_list_swap(&list, 1, 3);
    ALLC_TEST_ANY("4", "%zu", list.length);
    ALLC_TEST_ANY("10", "%d", *(int*)allc_list_at(&list, 0));
    ALLC_TEST_ANY("40", "%d", *(int*)allc_list_at(&list, 1)); // Was 20, now 40
    ALLC_TEST_ANY("30", "%d", *(int*)allc_list_at(&list, 2));
    ALLC_TEST_ANY("20", "%d", *(int*)allc_list_at(&list, 3)); // Was 40, now 20
    
    // Test: Try to swap elements of different sizes (should do nothing)
    double double_val = 3.14;
    allc_list_append(&list, sizeof(double), &double_val);
    
    // Before swap: list = [10, 40, 30, 20, 3.14]
    allc_list_swap(&list, 0, 4); // Try to swap int with double
    // After swap: should be unchanged
    ALLC_TEST_ANY("10", "%d", *(int*)allc_list_at(&list, 0));
    ALLC_TEST_ANY("3.14", "%.2f", *(double*)allc_list_at(&list, 4));
    
    allc_list_delete(&list);
}

void test__allc_list_move()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    // Setup list: [10, 20, 30, 40]
    int values[] = {10, 20, 30, 40};
    for (int i = 0; i < 4; i++) {
        allc_list_append(&list, sizeof(int), &values[i]);
    }
    
    // Test: Move element from index 1 to index 3
    allc_list_move(&list, 1, 3);
    ALLC_TEST_ANY("4", "%zu", list.length);
    ALLC_TEST_ANY("10", "%d", *(int*)allc_list_at(&list, 0));
    ALLC_TEST_ANY("30", "%d", *(int*)allc_list_at(&list, 1)); // 30 moved up
    ALLC_TEST_ANY("40", "%d", *(int*)allc_list_at(&list, 2)); // 40 moved up
    ALLC_TEST_ANY("20", "%d", *(int*)allc_list_at(&list, 3)); // 20 moved to end
    
    allc_list_delete(&list);
}

void test__allc_list_foreach_macro()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    // Setup list: [1, 2, 3]
    int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        allc_list_append(&list, sizeof(int), &values[i]);
    }
    
    // Test: FOREACH macro
    int sum = 0;
    ALLC_LIST_FOREACH(int, elem, &list, {
        sum += *elem;
    });
    ALLC_TEST_ANY("6", "%d", sum); // 1 + 2 + 3 = 6
    
    // Test: FOREACH_REVERSE macro
    int reverse_sum = 0;
    int multiplier = 1;
    ALLC_LIST_FOREACH_REVERSE(int, elem, &list, {
        reverse_sum += *elem * multiplier;
        multiplier *= 10;
    });
    ALLC_TEST_ANY("123", "%d", reverse_sum); // 3*1 + 2*10 + 1*100 = 123
    
    allc_list_delete(&list);
}

void test__allc_list_with_strings()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    // Test with string pointers
    const char *strings[] = {"Hello", "World", "List"};
    for (int i = 0; i < 3; i++) {
        allc_list_append(&list, sizeof(const char *), &strings[i]);
    }
    
    ALLC_TEST_ANY("3", "%zu", list.length);
    ALLC_TEST_ANY("Hello", "%s", *(const char**)allc_list_at(&list, 0));
    ALLC_TEST_ANY("World", "%s", *(const char**)allc_list_at(&list, 1));
    ALLC_TEST_ANY("List", "%s", *(const char**)allc_list_at(&list, 2));
    
    allc_list_delete(&list);
}

void test__allc_list_edge_cases()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    
    // Test: Operations on empty list
    allc_list_t empty_list = allc_list_new(allocator);
    ALLC_TEST_ANY("0", "%zu", empty_list.length);
    
    // Operations on empty list should be safe
    allc_list_remove(&empty_list, 0);
    ALLC_TEST_ANY("0", "%zu", empty_list.length);
    
    void *null_elem = allc_list_at(&empty_list, 0);
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(null_elem == NULL));
    
    allc_list_reverse(&empty_list);
    ALLC_TEST_ANY("0", "%zu", empty_list.length);
    
    allc_list_delete(&empty_list);
    
    // Test: Single element list operations
    allc_list_t single_list = allc_list_new(allocator);
    int val = 42;
    allc_list_append(&single_list, sizeof(int), &val);
    
    // Reverse single element
    allc_list_reverse(&single_list);
    ALLC_TEST_ANY("1", "%zu", single_list.length);
    ALLC_TEST_ANY("42", "%d", *(int*)allc_list_at(&single_list, 0));
    
    // Remove single element
    allc_list_remove(&single_list, 0);
    ALLC_TEST_ANY("0", "%zu", single_list.length);
    
    allc_list_delete(&single_list);
}

void test__allc_list_memory_safety()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    // Test: NULL safety
    allc_list_append(NULL, sizeof(int), &(int){10});
    allc_list_append(&list, sizeof(int), NULL);
    ALLC_TEST_ANY("0", "%zu", list.length);
    
    // Test: Multiple deletes should be safe
    allc_list_delete(&list);
    allc_list_delete(&list); // Should not crash
    ALLC_TEST_ANY("0", "%zu", list.length);
    
    allc_list_delete(NULL); // Should not crash
}

void test__allc_list_stress()
{
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_c();
    allc_list_t list = allc_list_new(allocator);
    
    const int num_entries = 1000;
    
    printf(">> Test stress - append operations:\n");
    // Add many entries
    for (int i = 0; i < num_entries; i++) {
        allc_list_append(&list, sizeof(int), &i);
    }
    
    ALLC_TEST_ANY("1000", "%zu", list.length);
    
    printf(">> Test stress - verify all entries exist:\n");
    // Verify all entries exist and have correct values
    bool success = true;
    for (int i = 0; success && i < num_entries; i++) {
        int *value = (int*)allc_list_at(&list, i);
        success &= (value != NULL);
        if (value) {
            success &= (*value == i);
        }
    }
    ALLC_TEST_BOOL(true, success);
    
    printf(">> Test stress - remove half the entries:\n");
    // Remove the second half of entries (indices 500-999)
    success = true;
    allc_size_t target_length = num_entries / 2;
    while (success && list.length > target_length) {
        allc_list_remove(&list, list.length - 1);
    }
    ALLC_TEST_ANY("500", "%zu", list.length);
    
    printf(">> Test stress - verify remaining entries:\n");
    // Verify remaining entries have correct values (should be 0, 1, 2, ..., 499)
    success = true;
    for (allc_size_t i = 0; success && i < list.length; i++) {
        int *value = (int*)allc_list_at(&list, i);
        success &= (value != NULL);
        if (value) {
            success &= (*value == (int)i);
        }
    }
    ALLC_TEST_BOOL(true, success);
    
    allc_list_delete(&list);
    
    printf("Stress test completed successfully\n");
}

int main()
{
    test__allc_list_new_delete();
    test__allc_list_append();
    test__allc_list_insert();
    test__allc_list_at_negative_indices();
    test__allc_list_remove();
    test__allc_list_reverse();
    test__allc_list_extend();
    test__allc_list_swap();
    test__allc_list_move();
    test__allc_list_foreach_macro();
    test__allc_list_with_strings();
    test__allc_list_edge_cases();
    test__allc_list_memory_safety();
    test__allc_list_stress();
    
    return 0;
}

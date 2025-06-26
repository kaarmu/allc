#define ALLC_IMPL

#include "../dev/strbuf.h"
#include "../dev/macro.h"

void test__allc_strbuf_is_equal(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer1 = allc_strbuf_new_from_cstr(allocator, "HelloWorld");
    StrBuf buffer2 = allc_strbuf_new_from_cstr(allocator, "HelloWorld");
    StrBuf buffer3 = allc_strbuf_new_from_cstr(allocator, "Hello");

    // Test: Two equal buffers
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_strbuf_is_equal(&buffer1, &buffer2)));

    // Test: Two different buffers
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_strbuf_is_equal(&buffer1, &buffer3)));

    allc_strbuf_delete(buffer1);
    allc_strbuf_delete(buffer2);
    allc_strbuf_delete(buffer3);
}

void test__allc_strbuf_is_equal_cstr(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new_from_cstr(allocator, "HelloWorld");

    // Test: Buffer equal to C-style string
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_strbuf_is_equal_cstr(&buffer, "HelloWorld")));

    // Test: Buffer different from C-style string
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_strbuf_is_equal_cstr(&buffer, "Hello")));

    allc_strbuf_delete(buffer);
}

void test__allc_strbuf_grow(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new(allocator, 10);

    // Test: Basic grow
    allc_strbuf_grow(&buffer, 5);
    ALLC_TEST_ANY("15", "%zu", buffer->capacity);

    // Test: Grow by zero (shouldn't change capacity)
    allc_strbuf_grow(&buffer, 0);
    ALLC_TEST_ANY("15", "%zu", buffer->capacity);

    allc_strbuf_delete(buffer);
}

void test__allc_strbuf_ensure_capacity(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new(allocator, 10);

    // Test: Ensure larger capacity
    allc_strbuf_ensure_capacity(&buffer, 15);
    ALLC_TEST_ANY("15", "%zu", buffer->capacity);

    // Test: Ensure smaller capacity (shouldn't change capacity)
    allc_strbuf_ensure_capacity(&buffer, 5);
    ALLC_TEST_ANY("15", "%zu", buffer->capacity);

    // Test: Ensure same capacity (shouldn't change capacity)
    allc_strbuf_ensure_capacity(&buffer, 15);
    ALLC_TEST_ANY("15", "%zu", buffer->capacity);

    allc_strbuf_delete(buffer);
}

void test__allc_strbuf_clear(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new_from_cstr(allocator, "HelloWorld");

    // Test: Clear content
    allc_strbuf_clear(&buffer);
    ALLC_TEST_ANY("0", "%zu", buffer->length);
    ALLC_TEST_ANY("\0", "%c", buffer->buf[0]);  // First character should be null terminator

    // Test: New empty buffer should be null-terminated
    StrBuf empty_buffer = allc_strbuf_new(allocator, 10);
    ALLC_TEST_ANY("0", "%zu", empty_buffer->length);
    ALLC_TEST_ANY("\0", "%c", empty_buffer->buf[0]);  // Should be null-terminated
    
    allc_strbuf_delete(buffer);
    allc_strbuf_delete(empty_buffer);
}

void test__allc_strbuf_set_fmt(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new(allocator, 10);  // Initial capacity of 10

    // Test: Basic formatting
    allc_strbuf_set_fmt(&buffer, "Hello %s!", "World");
    ALLC_TEST_ANY("Hello World!", "%s", buffer->buf);
    ALLC_TEST_ANY("12", "%zu", buffer->length);

    // Test: Formatting with numbers
    allc_strbuf_set_fmt(&buffer, "Number: %d", 42);
    ALLC_TEST_ANY("Number: 42", "%s", buffer->buf);
    ALLC_TEST_ANY("10", "%zu", buffer->length);

    // Test: Formatting with multiple placeholders
    allc_strbuf_set_fmt(&buffer, "%s %d %c", "Test", 123, 'A');
    ALLC_TEST_ANY("Test 123 A", "%s", buffer->buf);
    ALLC_TEST_ANY("10", "%zu", buffer->length);

    allc_strbuf_delete(buffer);
}

void test__allc_strbuf_set_cstr(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new(allocator, 10);  // Initial capacity of 10

    // Test: Set content to a C-style string
    allc_strbuf_set_cstr(&buffer, "HelloWorld");
    ALLC_TEST_ANY("HelloWorld", "%s", buffer->buf);

    // Test: Set content to an empty string
    allc_strbuf_set_cstr(&buffer, "");
    ALLC_TEST_ANY("", "%s", buffer->buf);
    ALLC_TEST_ANY("0", "%zu", buffer->length);

    allc_strbuf_delete(buffer);
}

void test__allc_strbuf_append_cstr(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new_from_cstr(allocator, "Hello");

    // Test: Basic append
    allc_strbuf_append_cstr(&buffer, " World");
    ALLC_TEST_ANY("Hello World", "%s", buffer->buf);

    // Test: Append an empty string (shouldn't change content)
    allc_strbuf_append_cstr(&buffer, "");
    ALLC_TEST_ANY("Hello World", "%s", buffer->buf);

    allc_strbuf_delete(buffer);
}

void test__allc_strbuf_insert_cstr(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new_from_cstr(allocator, "HelloWorld");

    // Test: Basic insert at a specific position
    allc_strbuf_insert_cstr(&buffer, 5, " ");
    ALLC_TEST_ANY("Hello World", "%s", buffer->buf);

    // Test: Insert at the beginning
    allc_strbuf_insert_cstr(&buffer, 0, "Start: ");
    ALLC_TEST_ANY("Start: Hello World", "%s", buffer->buf);

    // Test: Insert at the end
    allc_strbuf_insert_cstr(&buffer, buffer->length, "!");
    ALLC_TEST_ANY("Start: Hello World!", "%s", buffer->buf);

    // Test: Insert using a negative position (counted from the back)
    allc_strbuf_insert_cstr(&buffer, -6, " there");
    ALLC_TEST_ANY("Start: Hello  thereWorld!", "%s", buffer->buf);

    // Test: Insert with position greater than buffer length (should clamp and append to the end)
    allc_strbuf_insert_cstr(&buffer, 100, " [End]");
    ALLC_TEST_ANY("Start: Hello  thereWorld! [End]", "%s", buffer->buf);

    // Test: Insert with negative position beyond buffer start (should clamp and insert at the beginning)
    allc_strbuf_insert_cstr(&buffer, -100, "[Start] ");
    ALLC_TEST_ANY("[Start] Start: Hello  thereWorld! [End]", "%s", buffer->buf);

    allc_strbuf_delete(buffer);
}

void test__allc_strbuf_strip_blank(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new_from_cstr(allocator, "  Hello World  ");

    // Test: Strip both left and right
    allc_strbuf_strip_blank(&buffer);
    ALLC_TEST_ANY("Hello World", "%s", buffer->buf);

    allc_strbuf_delete(buffer);
}

void test__allc_strbuf_strip_left_blank(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new_from_cstr(allocator, "  Hello World");

    // Test: Strip only left
    allc_strbuf_strip_left_blank(&buffer);
    ALLC_TEST_ANY("Hello World", "%s", buffer->buf);

    allc_strbuf_delete(buffer);
}

void test__allc_strbuf_strip_right_blank(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new_from_cstr(allocator, "Hello World  ");

    // Test: Strip only right
    allc_strbuf_strip_right_blank(&buffer);
    ALLC_TEST_ANY("Hello World", "%s", buffer->buf);

    allc_strbuf_delete(buffer);
}

void test__allc_strbuf_split_at(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new_from_cstr(allocator, "HelloWorld");

    // Test: Split at a specific position
    struct allc_strbuf_pair_s pair = allc_strbuf_split_at(buffer, 5);
    ALLC_TEST_ANY("Hello", "%s", pair.left->buf);
    ALLC_TEST_ANY("World", "%s", pair.right->buf);

    allc_strbuf_delete(pair.left);
    allc_strbuf_delete(pair.right);

    // Test: Split at the beginning (left should be empty)
    buffer = allc_strbuf_new_from_cstr(allocator, "HelloWorld");
    pair = allc_strbuf_split_at(buffer, 0);
    ALLC_TEST_ANY("", "%s", pair.left->buf);
    ALLC_TEST_ANY("HelloWorld", "%s", pair.right->buf);

    allc_strbuf_delete(pair.left);
    allc_strbuf_delete(pair.right);

    // Test: Split at the end (right should be empty)
    buffer = allc_strbuf_new_from_cstr(allocator, "HelloWorld");
    pair = allc_strbuf_split_at(buffer, buffer->length);
    ALLC_TEST_ANY("HelloWorld", "%s", pair.left->buf);
    ALLC_TEST_ANY("", "%s", pair.right->buf);

    allc_strbuf_delete(pair.left);
    allc_strbuf_delete(pair.right);
}

void test__allc_strbuf_split_on_char(allc_allocator_t allocator) {
    printf("\n[%s]\n", __func__);

    StrBuf buffer = allc_strbuf_new_from_cstr(allocator, "Hello-World");

    // Test: Split on first occurrence of '-'
    struct allc_strbuf_pair_s pair = allc_strbuf_split_on_char(buffer, 1, '-');
    ALLC_TEST_ANY("Hello", "%s", pair.left->buf);
    ALLC_TEST_ANY("World", "%s", pair.right->buf);

    allc_strbuf_delete(pair.left);
    allc_strbuf_delete(pair.right);

    // Test: Split on non-existent character (right should be empty)
    buffer = allc_strbuf_new_from_cstr(allocator, "HelloWorld");
    pair = allc_strbuf_split_on_char(buffer, 1, '-');
    ALLC_TEST_ANY("HelloWorld", "%s", pair.left->buf);
    ALLC_TEST_ANY("", "%s", pair.right->buf);

    allc_strbuf_delete(pair.left);
    allc_strbuf_delete(pair.right);
}

int main() {
  allc_allocator_t allocator = allc_allocator_c();
  
  test__allc_strbuf_is_equal(allocator);
  test__allc_strbuf_is_equal_cstr(allocator);
  test__allc_strbuf_grow(allocator);
  test__allc_strbuf_ensure_capacity(allocator);
  test__allc_strbuf_clear(allocator);
  test__allc_strbuf_set_fmt(allocator);
  test__allc_strbuf_set_cstr(allocator);
  test__allc_strbuf_append_cstr(allocator);
  test__allc_strbuf_insert_cstr(allocator);
  test__allc_strbuf_strip_blank(allocator);
  test__allc_strbuf_strip_left_blank(allocator);
  test__allc_strbuf_strip_right_blank(allocator);
  test__allc_strbuf_split_at(allocator);
  test__allc_strbuf_split_on_char(allocator);
}

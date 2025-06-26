#define ALLC_IMPL

#include "../dev/macro.h"
#include "../dev/cstr.h"
#include "../dev/allocator.h"
#include "../dev/strbuf.h"

void allc_cstr_capitalize(char *str);
void allc_cstr_capitalize_all(char *str);
void allc_cstr_lower(char *str);
void allc_cstr_lower_all(char *str);
void allc_cstr_upper(char *str);
void allc_cstr_upper_all(char *str);
void allc_cstr_swap_case(char *str);
void allc_cstr_swap_case_all(char *str);

void test__allc_cstr_copy()
{
    printf("\n[%s]\n", __func__);

    char x[32];

    // Test: Basic copy
    allc_cstr_copy("HelloWorld", x);
    ALLC_TEST_ANY("HelloWorld", "%s", x);

    // Test: Copy an empty string
    allc_cstr_copy("", x);
    ALLC_TEST_ANY("", "%s", x);
}

void test__allc_cstr_copy_n()
{
    printf("\n[%s]\n", __func__);

    char x[32];

    // Test: Copy first `n` characters
    allc_cstr_copy_n("HelloWorld", 5, x);
    x[5] = '\0'; // Null-terminate after copying n characters
    ALLC_TEST_ANY("Hello", "%s", x);

    // Test: Copy more characters than the source string has
    allc_cstr_copy_n("Hello", 10, x);
    ALLC_TEST_ANY("Hello", "%s", x);

    // Test: Copy from an empty string
    allc_cstr_copy_n("", 5, x);
    ALLC_TEST_ANY("", "%s", x);
}

void test__inspection()
{
    printf("\n[%s]\n", __func__);

    // Test allc_cstr_is_blank
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_blank("    \t\t")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_blank("  a \t\t")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_blank("")));

    // Test allc_cstr_is_digit
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_digit("1234567890")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_digit("123a4567890")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_digit("")));

    // Test allc_cstr_is_upper
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_upper("ABCDEFGHIJKLMNOPQRSTUVWXYZ")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_upper("ABCDEFGHIJKLMNOPQRSTUVWXYZa")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_upper("")));

    // Test allc_cstr_is_lower
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_lower("abcdefghijklmnopqrstuvwxyz")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_lower("abcdefghijklmnopqrstuvwxyZ")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_lower("")));

    // Test allc_cstr_is_alpha
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_alpha("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_alpha("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_alpha("")));

    // Test allc_cstr_is_alnum
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_alnum("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_alnum("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_alnum("")));

    // Test allc_cstr_is_bin
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_bin("1010101010")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_bin("1012101010")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_bin("")));

    // Test allc_cstr_is_octo
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_octo("01234567")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_octo("01238967")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_octo("")));

    // Test allc_cstr_is_hexa
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_hexa("0123456789abcdefABCDEF")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_hexa("0123456789abcdefABCDEFg")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_hexa("")));

    // Test allc_cstr_is_equal
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_equal("testString", "testString")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_equal("testString1", "testString2")));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_equal("", "")));

    // Test allc_cstr_is_starting_with
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_starting_with("HelloWorld", "Hello")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_starting_with("HelloWorld", "World")));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_starting_with("HelloWorld", "")));

    // Test allc_cstr_is_ending_with
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_ending_with("HelloWorld", "World")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_ending_with("HelloWorld", "Hello")));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_ending_with("HelloWorld", "")));

    // Test allc_cstr_is_containing
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_containing("HelloWorld", "loW")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_containing("HelloWorld", "WorlD")));
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_containing("HelloWorld", "")));

    // Test allc_cstr_is_empty
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(allc_cstr_is_empty("")));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(allc_cstr_is_empty("HelloWorld")));

    // Test allc_cstr_length
    ALLC_TEST_ANY("10", "%zu", allc_cstr_length("HelloWorld"));
    ALLC_TEST_ANY("0", "%zu", allc_cstr_length(""));

    // Test allc_cstr_find_blank
    ALLC_TEST_ANY("5", "%zu", allc_cstr_find_blank("Hello World"));
    ALLC_TEST_ANY("10", "%zu", allc_cstr_find_blank("HelloWorld"));
    ALLC_TEST_ANY("0", "%zu", allc_cstr_find_blank(""));

    // Test allc_cstr_find_char
    ALLC_TEST_ANY("4", "%zu", allc_cstr_find_char("HelloWorld", 1, 'o'));
    ALLC_TEST_ANY("6", "%zu", allc_cstr_find_char("HelloWorld", 2, 'o'));
    ALLC_TEST_ANY("10", "%zu", allc_cstr_find_char("HelloWorld", 3, 'o'));
    ALLC_TEST_ANY("0", "%zu", allc_cstr_find_char("HelloWorld", 0, 'o'));
    ALLC_TEST_ANY("10", "%zu", allc_cstr_find_char("HelloWorld", 1, 'z'));

    // Test allc_cstr_find_cstr
    ALLC_TEST_ANY("4", "%zu", allc_cstr_find_cstr("HelloWorld", 1, "oW"));
    ALLC_TEST_ANY("0", "%zu", allc_cstr_find_cstr("HelloWorld", 0, "oW"));
    ALLC_TEST_ANY("10", "%zu", allc_cstr_find_cstr("HelloWorld", 1, "zZ"));
    ALLC_TEST_ANY("10", "%zu", allc_cstr_find_cstr("HelloWorld", 1, "Worlds"));
}

void test__representation()
{
    printf("\n[%s]\n", __func__);

    // Test: Representation of boolean true/false
    ALLC_TEST_ANY("true", "%s", allc_cstr_repr_bool(true));
    ALLC_TEST_ANY("false", "%s", allc_cstr_repr_bool(false));
}

void test__allc_cstr_remove()
{
    printf("\n[%s]\n", __func__);

    char x[16];

    // Test: Basic removal from the middle
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove(x, 1, 4);
    ALLC_TEST_ANY("HoWorld", "%s", x);

    // Test: Removing from the start
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove(x, 0, 5);
    ALLC_TEST_ANY("World", "%s", x);

    // Test: Removing from the end
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove(x, 5, 10);
    ALLC_TEST_ANY("Hello", "%s", x);

    // Test: Removing entire string
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove(x, 0, 10);
    ALLC_TEST_ANY("", "%s", x);

    // Test: Removing with 'from' greater than 'to' (should do nothing)
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove(x, 7, 5);
    ALLC_TEST_ANY("HelloWorld", "%s", x);

    // Test: Removing with 'from' and 'to' out of bounds (should do nothing)
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove(x, 15, 20);
    ALLC_TEST_ANY("HelloWorld", "%s", x);

    // Test: Removing with 'to' out of bounds (should remove from 'from' to end of string)
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove(x, 5, 20);
    ALLC_TEST_ANY("Hello", "%s", x);
}

void test__allc_cstr_remove_suffix()
{
    printf("\n[%s]\n", __func__);

    char x[16];

    // Test: Basic removal of a known suffix
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove_suffix(x, "World");
    ALLC_TEST_ANY("Hello", "%s", x);

    // Test: Removing a suffix that doesn't match (should do nothing)
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove_suffix(x, "Hello");
    ALLC_TEST_ANY("HelloWorld", "%s", x);

    // Test: Removing a suffix from an empty string (should do nothing)
    allc_cstr_copy("", x);
    allc_cstr_remove_suffix(x, "World");
    ALLC_TEST_ANY("", "%s", x);

    // Test: Removing an empty suffix (should do nothing)
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove_suffix(x, "");
    ALLC_TEST_ANY("HelloWorld", "%s", x);

    // Test: Removing a suffix longer than the string (should do nothing)
    allc_cstr_copy("Hello", x);
    allc_cstr_remove_suffix(x, "HelloWorld");
    ALLC_TEST_ANY("Hello", "%s", x);
}

void test__allc_cstr_remove_prefix()
{
    printf("\n[%s]\n", __func__);

    char x[16];

    // Test: Basic removal of a known prefix
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove_prefix(x, "Hello");
    ALLC_TEST_ANY("World", "%s", x);

    // Test: Removing a prefix that doesn't match (should do nothing)
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove_prefix(x, "World");
    ALLC_TEST_ANY("HelloWorld", "%s", x);

    // Test: Removing a prefix from an empty string (should do nothing)
    allc_cstr_copy("", x);
    allc_cstr_remove_prefix(x, "Hello");
    ALLC_TEST_ANY("", "%s", x);

    // Test: Removing an empty prefix (should do nothing)
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_remove_prefix(x, "");
    ALLC_TEST_ANY("HelloWorld", "%s", x);

    // Test: Removing a prefix longer than the string (should do nothing)
    allc_cstr_copy("World", x);
    allc_cstr_remove_prefix(x, "HelloWorld");
    ALLC_TEST_ANY("World", "%s", x);
}

void test__allc_cstr_strip_left_blank()
{
    printf("\n[%s]\n", __func__);

    char x[32];
    size_t removed;

    // Test: Basic removal of leading blank characters
    allc_cstr_copy("   HelloWorld", x);
    removed = allc_cstr_strip_left_blank(x);
    ALLC_TEST_ANY("HelloWorld", "%s", x);
    ALLC_TEST_ANY("3", "%zu", removed);

    // Test: No leading blank characters to remove
    allc_cstr_copy("HelloWorld", x);
    removed = allc_cstr_strip_left_blank(x);
    ALLC_TEST_ANY("HelloWorld", "%s", x);
    ALLC_TEST_ANY("0", "%zu", removed);

    // Test: All characters are blank
    allc_cstr_copy("     ", x);
    removed = allc_cstr_strip_left_blank(x);
    ALLC_TEST_ANY("", "%s", x);
    ALLC_TEST_ANY("5", "%zu", removed);
}

void test__allc_cstr_strip_right_blank()
{
    printf("\n[%s]\n", __func__);

    char x[32];
    size_t removed;

    // Test: Basic removal of trailing blank characters
    allc_cstr_copy("HelloWorld   ", x);
    removed = allc_cstr_strip_right_blank(x);
    ALLC_TEST_ANY("HelloWorld", "%s", x);
    ALLC_TEST_ANY("3", "%zu", removed);

    // Test: No trailing blank characters to remove
    allc_cstr_copy("HelloWorld", x);
    removed = allc_cstr_strip_right_blank(x);
    ALLC_TEST_ANY("HelloWorld", "%s", x);
    ALLC_TEST_ANY("0", "%zu", removed);

    // Test: All characters are blank
    allc_cstr_copy("     ", x);
    removed = allc_cstr_strip_right_blank(x);
    ALLC_TEST_ANY("", "%s", x);
    ALLC_TEST_ANY("5", "%zu", removed);
}

void test__allc_cstr_strip_blank()
{
    printf("\n[%s]\n", __func__);

    char x[32];
    size_t removed;

    // Test: Basic removal of leading and trailing blank characters
    allc_cstr_copy("   HelloWorld   ", x);
    removed = allc_cstr_strip_blank(x);
    ALLC_TEST_ANY("HelloWorld", "%s", x);
    ALLC_TEST_ANY("6", "%zu", removed);

    // Test: No blank characters to remove
    allc_cstr_copy("HelloWorld", x);
    removed = allc_cstr_strip_blank(x);
    ALLC_TEST_ANY("HelloWorld", "%s", x);
    ALLC_TEST_ANY("0", "%zu", removed);

    // Test: All characters are blank
    allc_cstr_copy("     ", x);
    removed = allc_cstr_strip_blank(x);
    ALLC_TEST_ANY("", "%s", x);
    ALLC_TEST_ANY("5", "%zu", removed);
}

void test__allc_cstr_shift_left()
{
    printf("\n[%s]\n", __func__);

    char x[32];

    // Test: Basic left shift
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_shift_left(x, 5);
    ALLC_TEST_ANY("World", "%s", x);

    // Test: Left shift with `n` as zero (should do nothing)
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_shift_left(x, 0);
    ALLC_TEST_ANY("HelloWorld", "%s", x);

    // Test: Left shift greater than string length (should remove everything)
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_shift_left(x, 15);
    ALLC_TEST_ANY("", "%s", x);
}

void test__allc_cstr_shift_right()
{
    printf("\n[%s]\n", __func__);

    char x[32];

    // Test: Basic right shift
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_shift_right(x, 5);
    ALLC_TEST_ANY("Hello", "%s", x);

    // Test: Right shift with `n` as zero (should do nothing)
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_shift_right(x, 0);
    ALLC_TEST_ANY("HelloWorld", "%s", x);

    // Test: Right shift greater than string length (should remove everything)
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_shift_right(x, 15);
    ALLC_TEST_ANY("", "%s", x);
}

void test__allc_cstr_replace_char()
{
    printf("\n[%s]\n", __func__);

    char x[64];

    // Basic replacement of the first `n` characters
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_replace_char(x, 1, 'o', 'x');
    ALLC_TEST_ANY("HellxWorld", "%s", x);

    // Replace multiple characters
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_replace_char(x, 2, 'l', 'x');
    ALLC_TEST_ANY("HexxoWorld", "%s", x);

    // Character not found
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_replace_char(x, 1, 'z', 'x');
    ALLC_TEST_ANY("HelloWorld", "%s", x);
}

void test__allc_cstr_replace_all_char()
{
    printf("\n[%s]\n", __func__);

    char x[64];

    // Basic replacement of all matching characters
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_replace_all_char(x, 'o', 'x');
    ALLC_TEST_ANY("HellxWxrld", "%s", x);

    // Character not found
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_replace_all_char(x, 'z', 'x');
    ALLC_TEST_ANY("HelloWorld", "%s", x);
}

void test__allc_cstr_replace_cstr()
{
    printf("\n[%s]\n", __func__);

    char x[64];

    // Basic replacement of the first `n` substrings
    allc_cstr_copy("HelloWorldHello", x);
    allc_cstr_replace_cstr(x, 1, "Hello", "Hi");
    ALLC_TEST_ANY("HiWorldHello", "%s", x);

    // Replace multiple substrings
    allc_cstr_copy("HelloWorldHello", x);
    allc_cstr_replace_cstr(x, 2, "Hello", "Hi");
    ALLC_TEST_ANY("HiWorldHi", "%s", x);

    // Substring not found
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_replace_cstr(x, 1, "Bye", "Hi");
    ALLC_TEST_ANY("HelloWorld", "%s", x);

    // New substring is larger than the old one
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_replace_cstr(x, 1, "Hello", "Greetings");
    ALLC_TEST_ANY("HelloWorld", "%s", x);
}

void test__allc_cstr_replace_all_cstr()
{
    printf("\n[%s]\n", __func__);

    char x[64];

    // Basic replacement of all matching substrings
    allc_cstr_copy("HelloWorldHello", x);
    allc_cstr_replace_all_cstr(x, "Hello", "Hi");
    ALLC_TEST_ANY("HiWorldHi", "%s", x);

    // Substring not found
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_replace_all_cstr(x, "Bye", "Hi");
    ALLC_TEST_ANY("HelloWorld", "%s", x);

    // New substring is larger than the old one
    allc_cstr_copy("HelloWorld", x);
    allc_cstr_replace_all_cstr(x, "Hello", "Greetings");
    ALLC_TEST_ANY("HelloWorld", "%s", x);
}

int main()
{
    test__allc_cstr_copy();
    test__allc_cstr_copy_n();
    test__inspection();
    test__representation();
    test__allc_cstr_remove();
    test__allc_cstr_remove_suffix();
    test__allc_cstr_remove_prefix();
    test__allc_cstr_strip_left_blank();
    test__allc_cstr_strip_right_blank();
    test__allc_cstr_strip_blank();
    test__allc_cstr_shift_left();
    test__allc_cstr_shift_right();
    test__allc_cstr_replace_char();
    test__allc_cstr_replace_all_char();
    test__allc_cstr_replace_cstr();
    test__allc_cstr_replace_all_cstr();
}

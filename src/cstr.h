/**
 *  C-style strings.
 *
 *  Description
 *  ===========
 *
 *  Options
 *  =======
 *
 *  Content
 *  =======
 *
 *  1) String Inspection
 *
 *      [x] allc_cstr_is_blank
 *      [x] allc_cstr_is_digit
 *      [x] allc_cstr_is_upper
 *      [x] allc_cstr_is_lower
 *      [x] allc_cstr_is_alpha
 *      [x] allc_cstr_is_alnum
 *      [x] allc_cstr_is_bin
 *      [x] allc_cstr_is_octo
 *      [x] allc_cstr_is_hexa
 *      [x] allc_cstr_is_equal
 *      [x] allc_cstr_is_starting_with
 *      [x] allc_cstr_is_ending_with
 *      [x] allc_cstr_length
 *      [x] allc_cstr_find_blank
 *      [x] allc_cstr_find_char
 *      [x] allc_cstr_find_cstr
 *
 *  2) String Manipulation
 *
 *      [ ] allc_cstr_set_char
 *      [ ] allc_cstr_capitalize
 *      [ ] allc_cstr_capitalize_all
 *      [ ] allc_cstr_lower
 *      [ ] allc_cstr_lower_all
 *      [ ] allc_cstr_upper
 *      [ ] allc_cstr_upper_all
 *      [ ] allc_cstr_swap_case
 *      [ ] allc_cstr_swap_case_all
 *      [x] allc_cstr_remove
 *      [x] allc_cstr_remove_suffix
 *      [x] allc_cstr_remove_prefix
 *      [x] allc_cstr_strip_left_blank
 *      [x] allc_cstr_strip_right_blank
 *      [x] allc_cstr_strip_blank
 *      [x] allc_cstr_shift_left
 *      [x] allc_cstr_shift_right
 *      [x] allc_cstr_replace_char
 *      [x] allc_cstr_replace_all_char
 *      [x] allc_cstr_replace_cstr
 *      [x] allc_cstr_replace_all_cstr
 *
 *  3) String Representation
 *
 *      [ ] allc_cstr_repr_int
 *      [ ] allc_cstr_repr_float
 *      [x] allc_cstr_repr_bool
 *
 *  4) Other
 *
 *      [x] allc_cstr_copy
 *      [x] allc_cstr_copy_n
 **/

#ifndef ALLC_CSTR_GUARD
#define ALLC_CSTR_GUARD

// Includes {{{1
// -------------

#include <sys/types.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>


// String Inspection {{{1
// ----------------------

/* Test that string contains only space or tab characters. */
bool allc_cstr_is_blank(const char *str);

/**
 * Test that string contains only characters in '0'..'9'.
 *
 * Returns false on empty string
 **/
bool allc_cstr_is_digit(const char *str);

/**
 * Test that string contains only characters in 'A'..'Z'.
 *
 * Returns false on empty string
 **/
bool allc_cstr_is_upper(const char *str);

/* Test that string contains only characters in 'a'..'z'. */
bool allc_cstr_is_lower(const char *str);

/* Test that string contains only characters in 'A'..'Z' or 'a'..'z'. */
bool allc_cstr_is_alpha(const char *str);

/**
 * Test that string contains only characters in '0'..'9', 'A'..'Z' or
 * 'a'..'z'.
 **/
bool allc_cstr_is_alnum(const char *str);

/**
 * Test that string contains only characters '0' or '1'.
 *
 * Returns false on empty string
 **/
bool allc_cstr_is_bin(const char *str);

/**
 * Test that string contains only characters in '0'..'7'.
 *
 * Returns false on empty string
 **/
bool allc_cstr_is_octo(const char *str);

/**
 * Test that string contains only characters in '0'..'9', 'a'..'f' or 'A'..'F'.
 *
 * Returns false on empty string
 **/
bool allc_cstr_is_hexa(const char *str);

/* Test that the two strings are equal. */
bool allc_cstr_is_equal(const char *str1, const char *str2);

/**
 * Test that string starts with substring.
 *
 * If substring is empty, return true.
 **/
bool allc_cstr_is_starting_with(const char *str, const char *sub);

/**
 * Test that string starts with substring.
 *
 * If substring is empty, return true.
 **/
bool allc_cstr_is_ending_with(const char *str, const char *sub);

/**
 * Return length of string.
 *
 * Length does not include the null terminator.
 **/
size_t allc_cstr_length(const char *str);

/**
 * Find the index of the first occurrence of a blank character in string.
 *
 * If a blank character cannot be found, the string length is returned.
 **/
size_t allc_cstr_find_blank(const char *str);

/**
 * Find the index of the `n`th occurrence of character `c` in string.
 *
 * If `n` is zero, return 0.
 * If the character cannot be found, return the string length.
 **/
size_t allc_cstr_find_char(const char *str, ssize_t n, const char chr);

/**
 * Find the index of the `n`th occurence of substring `sub` in string.
 *
 * If `n` is zero, return 0.
 * If the character cannot be found, the string length is returned.
 **/
size_t allc_cstr_find_cstr(const char *str, ssize_t n, const char *sub);


// String Manipulation {{{1
// ------------------------

/**
 * Remove chracters between index `from` up to, but not including, index
 * `to`.
 **/
void allc_cstr_remove(char *str, size_t from, size_t to);

/* Remove a known suffix. */
void allc_cstr_remove_suffix(char *str, const char *suffix);

/* Remove a known prefix. */
void allc_cstr_remove_prefix(char *str, const char *prefix);

/**
 * Remove all blank characters that begin the string.
 *
 * Returns the number of characters that were removed.
 **/
size_t  allc_cstr_strip_left_blank(char *str);

/**
 * Remove all blank characters that end the string.
 *
 * Returns the number of characters that were removed.
 **/
size_t  allc_cstr_strip_right_blank(char *str);

/**
 * Remove all blank characters at both beginning and end of the string.
 *
 * Returns the number of characters that were removed.
 **/
size_t  allc_cstr_strip_blank(char *str_buf);

/**
 * Left shift string.
 *
 * If `n` is zero, do nothing.
 * If `n` is greater than string length, remove everything.
 **/
void allc_cstr_shift_left(char *str, size_t n);

/**
 * Right shift string.
 *
 * If `n` is zero, do nothing.
 * If `n` is greater than string length, remove everything.
 **/
void allc_cstr_shift_right(char *str, size_t n);

/**
 * Replace the first `n` characters matching `chr` in a string.
 *
 * If the character cannot be found, do nothing.
 **/
void allc_cstr_replace_char(char *str, size_t n, const char chr, const char rpl);

/**
 * Replace all characters matching `chr` in a string.
 *
 * If the character cannot be found, do nothing.
 **/
void allc_cstr_replace_all_char(char *str, const char chr, const char rpl);

/**
 * Replace the first `n` substrings matching `sub` in a string.
 *
 * If the new substring is larger than the old, do nothing.
 * If the old substring is larger than the new, replace as much as possible.
 * If the substring cannot be found, do nothing.
 **/
void allc_cstr_replace_cstr(char *str, size_t n, const char *sub, const char *rpl);

/**
 * Replace all substrings matching `sub` in a string.
 *
 * If the new substring is larger than the old, do nothing.
 * If the old substring is larger than the new, replace as much as possible.
 * If the substring cannot be found, do nothing.
 **/
void allc_cstr_replace_all_cstr(char *str, const char *sub, const char *rpl);


// String Representation {{{1
// --------------------------

const char *allc_cstr_repr_bool(bool b);


// Other {{{1
// ----------

/* Copy entire C-style string `from` to `to`. */
void allc_cstr_copy(const char *from, char *to);

/* Copy at most `n` characters from C-style string `from` to `to`. */
void allc_cstr_copy_n(const char *from, size_t n, char *to);


#endif // ALLC_CSTR_GUARD

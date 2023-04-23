/*
 *  STB-style header library of C-style string handling.
 *
 *  Description
 *  ===========
 *
 *  Provides
 *  ========
 *
 *  Options
 *  =======
 *
 *  Library
 *  =======
 *
 *  Version: 0.0.0
 *
 *  Author: Kaj Munhoz Arfvidsson
 */

#ifndef ALLC_CSTR_GUARD // {{{1
#define ALLC_CSTR_GUARD

#include <sys/types.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// String Inspection {{{2

/* Test that string contains only space or tab characters. */
bool allc_cstr_is_blank(const char *str);

/*
 * Test that string contains only characters in '0'..'9'.
 *
 * Returns false on empty string
 */
bool allc_cstr_is_digit(const char *str);

/*
 * Test that string contains only characters in 'A'..'Z'.
 *
 * Returns false on empty string
 */
bool allc_cstr_is_upper(const char *str);

/*
 * Test that string contains only characters in 'a'..'z'.
 */
bool allc_cstr_is_lower(const char *str);

/*
 * Test that string contains only characters in 'A'..'Z' or 'a'..'z'.
 */
bool allc_cstr_is_alpha(const char *str);

/*
 * Test that string contains only characters in '0'..'9', 'A'..'Z' or 'a'..'z'.
 */
bool allc_cstr_is_alnum(const char *str);

/*
 * Test that string contains only characters '0' or '1'.
 *
 * Returns false on empty string
 */
bool allc_cstr_is_bin(const char *str);

/*
 * Test that string contains only characters in '0'..'7'.
 *
 * Returns false on empty string
 */
bool allc_cstr_is_octo(const char *str);

/*
 * Test that string contains only characters in '0'..'9', 'a'..'f' or 'A'..'F'.
 *
 * Returns false on empty string
 */
bool allc_cstr_is_hexa(const char *str);

/*
 * Test that the two strings are equal.
 */
bool allc_cstr_is_equal(const char *str1, const char *str2);

/*
 * Test that string starts with substring.
 *
 * If substring is empty, return true.
 */
bool allc_cstr_is_starting_with(const char *str, const char *sub);

/*
 * Test that string starts with substring.
 *
 * If substring is empty, return true.
 */
bool allc_cstr_is_ending_with(const char *str, const char *sub);

/*
 * Return length of string.
 *
 * Length does not include the null terminator.
 */
size_t allc_cstr_length(const char *str);

/*
 * Find the index of the first occurrence of a blank character in string.
 *
 * If a blank character cannot be found, the string length is returned.
 */
size_t allc_cstr_find_blank(const char *str);

/*
 * Find the index of the `n`th occurrence of character `c` in string.
 *
 * If `n` is zero, return 0.
 * If the character cannot be found, return the string length.
 */
size_t allc_cstr_find_char(const char *str, ssize_t n, const char chr);

/*
 * Find the index of the `n`th occurence of substring `sub` in string.
 *
 * If `n` is zero, return 0.
 * If the character cannot be found, the string length is returned.
 */
size_t allc_cstr_find_cstr(const char *str, ssize_t n, const char *sub);

// String Manipulation {{{2
// set_char
// capitalize
// capitalize_all
// lower
// lower_all
// upper
// upper_all
// swap_case
// swap_case_all

/*
 * Left shift string.
 *
 * If `n` is zero, do nothing.
 * If `n` is greater than string length, remove everything.
 */
void allc_cstr_shift_left(char *str, size_t n);

/*
 * Right shift string.
 *
 * If `n` is zero, do nothing.
 * If `n` is greater than string length, remove everything.
 */
void allc_cstr_shift_right(char *str, size_t n);

/*
 * Replace the first `n` characters matching `chr` in a string.
 *
 * If the character cannot be found, do nothing.
 */
void allc_cstr_replace_char(char *str, size_t n, const char chr, const char rpl);

/*
 * Replace all characters matching `chr` in a string.
 *
 * If the character cannot be found, do nothing.
 */
void allc_cstr_replace_all_char(char *str, const char chr, const char rpl);

/*
 * Replace the first `n` substrings matching `sub` in a string.
 *
 * If the new substring is larger than the old, do nothing.
 * If the old substring is larger than the new, replace as much as possible.
 * If the substring cannot be found, do nothing.
 */
void allc_cstr_replace_cstr(char *str, size_t n, const char *sub, const char *rpl);

/*
 * Replace all substrings matching `sub` in a string.
 *
 * If the new substring is larger than the old, do nothing.
 * If the old substring is larger than the new, replace as much as possible.
 * If the substring cannot be found, do nothing.
 */
void allc_cstr_replace_all_cstr(char *str, const char *sub, const char *rpl);

// String Representation {{{2
// repr_int
// repr_float

const char *allc_cstr_repr_bool(bool b);

// Other {{{2

/* Copy entire C-style string `from` to `to`. */
void allc_cstr_copy(const char *from, char *to);

/* Copy at most `n` characters from C-style string `from` to `to`. */
void allc_cstr_copy_n(const char *from, size_t n, char *to);

#endif // ALLC_CSTR_GUARD }}}1

#ifdef ALLC_IMPL_CSTR // {{{1
#ifndef ALLC_IMPL_CSTR_GUARD
#define ALLC_IMPL_CSTR_GUARD

// Macros {{{2

#define ALLC_CSTR_TRUE "true"
#define ALLC_CSTR_FALSE "false"

bool allc_cstr_is_blank(const char *str)
{
    bool result = true;
    for (const char *p = str; *p != 0; ++p) {
        switch (*p) {
        case ' ':
        case '\t':
            continue;
        default:
            result = false;
        }
    }
    return result;
}

bool allc_cstr_is_digit(const char *str)
{
    bool result = *str != 0;
    for (const char *p = str; *p != 0; ++p) {
        char chr = *p;
        result = result && (
            ('0' <= chr && chr <= '9')
        );
    }
    return result;
}

bool allc_cstr_is_upper(const char *str)
{
    bool result = *str != 0;
    for (const char *p = str; *p != 0; ++p) {
        char chr = *p;
        result = result && (
            ('A' <= chr && chr <= 'Z')
        );
    }
    return result;
}

bool allc_cstr_is_lower(const char *str)
{
    bool result = true;
    for (const char *p = str; *p != 0; ++p) {
        char chr = *p;
        result = result && (
            ('a' <= chr && chr <= 'z')
        );
    }
    return result;
}

bool allc_cstr_is_alpha(const char *str)
{
    bool result = true;
    for (const char *p = str; *p != 0; ++p) {
        char chr = *p;
        result = result && (
            ('A' <= chr && chr <= 'Z') || ('a' <= chr && chr <= 'z')
        );
    }
    return result;
}

bool allc_cstr_is_alnum(const char *str)
{
    bool result = true;
    for (const char *p = str; *p != 0; ++p) {
        char chr = *p;
        result = result && (
            ('0' <= chr && chr <= '9') || ('A' <= chr && chr <= 'Z') || ('a' <= chr && chr <= 'z')
        );
    }
    return result;
}

bool allc_cstr_is_bin(const char *str)
{
    bool result = *str != 0;
    for (const char *p = str; *p != 0; ++p) {
        char chr = *p;
        result = result && (
            (chr == '0' || chr == '1')
        );
    }
    return result;
}

bool allc_cstr_is_octo(const char *str)
{
    bool result = *str != 0;
    for (const char *p = str; *p != 0; ++p) {
        char chr = *p;
        result = result && (
            ('0' <= chr && chr <= '7')
        );
    }
    return result;
}

bool allc_cstr_is_hexa(const char *str)
{
    bool result = *str != 0;
    for (const char *p = str; *p != 0; ++p) {
        char chr = *p;
        result = result && (
            ('0' <= chr && chr <= '9') || ('A' <= chr && chr <= 'F') || ('a' <= chr && chr <= 'f')
        );
    }
    return result;
}

bool allc_cstr_is_equal(const char *str1, const char *str2)
{
    const char *p = str1, *q = str2;
    for (;*p != 0 && *q != 0; ++p, ++q) {
        if (*p != *q) return false;
    }
    return *p == *q;
}

bool allc_cstr_is_starting_with(const char *str, const char *sub)
{
    const char *p = str, *q = sub;
    for (; *p != 0 && *q != 0; ++p, ++q) {
        if (*p != *q) return false;
    }
    return true;
}

bool allc_cstr_is_ending_with(const char *str, const char *sub)
{
    size_t
        str_length = allc_cstr_length(str),
        sub_length = allc_cstr_length(sub);
    const char *p = str + str_length - sub_length;
    return allc_cstr_is_equal(p, sub);
}

size_t allc_cstr_length(const char *str)
{
    const char *p = str;
    for (; *p != 0; ++p);
    return (size_t)(p-str);
}

size_t  allc_cstr_find_blank(const char *str)
{
    const char *p = str;
    while (*p != 0 && !isblank(*p)) {
        ++p;
    }
    return p-str;
}

size_t allc_cstr_find_char(const char *str, ssize_t n, const char chr)
{
    const char *p = str;
    if (0 < n) {
        do {
            for (; *p != 0 && *p != chr; ++p);
        } while(--n > 0 && *p != 0 && *(++p) != 0);
    } else {
        for (; *p != 0; ++p);
        while(++n > 0 && p != str) {
            --p;
            for (; p != str && *p != chr; --p);
        }
    }
    return p - str;
}

size_t allc_cstr_find_cstr(const char *str, ssize_t n, const char *sub)
{
    size_t length = allc_cstr_length(str);
    if (*sub == 0) return length;
    const char *p, *q;
    int inc;
    if (n < 0) {
        p = str + length - 1;
        q = str - 1;
        inc = +1;
    } else {
        p = str;
        q = str + length;
        inc = -1;
    }
    while (p != q && n != 0) {
        if (allc_cstr_is_starting_with(p, sub)) {
            n += inc;
        } else {
            p += -inc;
        }
    }
    return n == 0 ? p - str : length;
}

void allc_cstr_shift_left(char *str, size_t n)
{
    size_t length = allc_cstr_length(str);
    char *p = str;
    for (; p + n < str + length; ++p) {
        *p = *(p + n);
    }
    if (p < str + length) memset(p, 0, str + length - p);
}

void allc_cstr_shift_right(char *str, size_t n)
{
    size_t length = allc_cstr_length(str);
    char *end = str + length;
    for (char *p = end-n; p < end; ++p) {
        *p = 0;
    }
}

void allc_cstr_replace_char(char *str, size_t n, const char chr, const char rpl)
{
    size_t i = 0;
    while(n != 0) {
        i = allc_cstr_find_char(str, 1, chr);
        if (str[i] == 0) return; // if chr not found then i = length wich implies str[i] = 0.
        str[i] = rpl;
        n -= 1;
        i += 1;
    }
}

void allc_cstr_replace_cstr(char *str, size_t n, const char *sub, const char *rpl)
{
    size_t
        i = 0,
        sub_length = allc_cstr_length(sub),
        rpl_length = allc_cstr_length(rpl);
    while (n != 0) {
        i = allc_cstr_find_cstr(str, 1, sub);
        if (str[i] == 0) return; // if sub not found then i = length wich implies str[i] = 0.
        allc_cstr_copy_n(rpl, rpl_length, str + i);
        allc_cstr_shift_left(str + i + rpl_length, sub_length - rpl_length);
        n -= 1;
        i += rpl_length;
    }
}

const char *allc_cstr_repr_bool(bool b)
{
    return b ? ALLC_CSTR_TRUE : ALLC_CSTR_FALSE;
}

void allc_cstr_copy(const char *from, char *to)
{
    strcpy(to, from);
}

void allc_cstr_copy_n(const char *from, size_t n, char *to)
{
    strncpy(to, from, n);
}

#endif // ALLC_IMPL_CSTR_GUARD
#endif // ALLC_IMPL_CSTR }}}1

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

#include <stdbool.h>
#include <stddef.h>

// String Inspection {{{2

/*
 * Return length of string.
 *
 * Length does not include the null terminator.
 *
 * See also: allc_cstr_size
 */
size_t allc_cstr_length(const char *str);

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
 * Find the first occurrence of character `c` in string.
 *
 * If the character cannot be found, the string length is returned.
 */
size_t allc_cstr_find_char(const char *str, const char chr);

/*
 * Find the first occurrence of substring `sub` in string.
 *
 * If the substring is empty, the string length is returned.
 * If the substring cannot be found, the string length is returned.
 */
size_t allc_cstr_find_cstr(const char *str, const char *sub);

/*
 * Find the Nth occurrence of character `c` in string.
 *
 * If the character cannot be found, the string length is returned.
 */
size_t allc_cstr_findn_char(const char *str, int n, const char chr);

// String Manipulation {{{2

/*
 * Replace a single character in a string.
 *
 * If the character cannot be found, do nothing.
 */
void allc_cstr_replace_char(char *str, const char chr, const char rpl);

/*
 * Replace a substring in a string.
 *
 * If the new substring is larger than the old, do nothing.
 * If the old substring is larger than the new, replace as much as possible.
 * If the substring cannot be found, do nothing.
 */
void allc_cstr_replace_cstr(char *str, const char *sub, const char *rpl);

// String Representation {{{2

const char *allc_cstr_repr_bool(bool b);

#endif // ALLC_CSTR_GUARD }}}1

#ifdef ALLC_IMPL_CSTR // {{{1
#ifndef ALLC_IMPL_CSTR_GUARD
#define ALLC_IMPL_CSTR_GUARD

// Macros {{{2

#define ALLC_CSTR_TRUE "true"

#define ALLC_CSTR_FALSE "false"

// String Inspection {{{2

size_t allc_cstr_length(const char *str)
{
    const char *p = str;
    for (; *p != 0; ++p);
    return (size_t)(p-str);
}

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

bool allc_cstr_is_starting_with(const char *str, const char *sub)
{
    const char *p = str, *q = sub;
    for (; *p != 0 && *q != 0; ++p, ++q) {
        if (*p != *q) return false;
    };
    return true;
}

bool allc_cstr_is_ending_with(const char *str, const char *sub)
{
    const char *p = str + allc_cstr_length(str), *q = sub + allc_cstr_length(sub);
    for (; *p != 0 && *q != 0; --p, --q) {
        if (*p != *q) return false;
    };
    return true;
}

size_t allc_cstr_find_char(const char *str, const char chr)
{
    const char *p = str;
    for (; *p != 0 && *p != chr; ++p);
    return (size_t)(p-str);
}

size_t allc_cstr_findn_char(const char *str, int n, const char chr)
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
    return (size_t)(p-str);
}

size_t allc_cstr_find_cstr(const char *str, const char *sub)
{
    size_t i = allc_cstr_find_char(str, sub[0]);
    if (str[i] == 0) return i;
    for (size_t j = 1; str[i+j] != 0 && sub[j] != 0; ++j) {
        if (str[i+j] != sub[j]) {
            i += allc_cstr_find_char(&str[i+1], sub[0]) + 1;
            j = 1;
            if (str[i] == 0) return i;
        }
    };
    return i;
}

/**
 * Return size of string.
 *
 * Size includes the null terminator.
 *
 * See also: allc_cstr_length
 */
size_t allc_cstr_size(const char *str)
{
    return allc_cstr_length(str) + 1;
}

// String Manipulation {{{2

void allc_cstr_replace_char(char *str, const char chr, const char rpl)
{
    size_t i = allc_cstr_find_char(str, chr);
    if (str[i] == 0) return; // couldn't find
    str[i] = rpl;
}

void allc_cstr_replace_cstr(char *str, const char *sub, const char *rpl)
{
    if (allc_cstr_length(sub) < allc_cstr_length(rpl)) return;
    char *p = str + allc_cstr_find_cstr(str, sub);
    if (*p == 0) return;
    for (const char *q = rpl; *q != 0; ++p, ++q)
        *p = *q;
}

// String Representation {{{2

const char *allc_cstr_repr_bool(bool b)
{
    return b ? ALLC_CSTR_TRUE : ALLC_CSTR_FALSE;
}

#endif // ALLC_IMPL_CSTR_GUARD
#endif // ALLC_IMPL_CSTR }}}1

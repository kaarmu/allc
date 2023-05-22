#include "../include/cstr.h"

// Macros {{{

#define ALLC_CSTR_TRUE "true"
#define ALLC_CSTR_FALSE "false"

// String Inspection {{{

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

// String Manipulation {{{

void allc_cstr_remove(char *str, size_t from, size_t to)
{
    allc_cstr_shift_left(str + from, to - from);
}

void allc_cstr_remove_suffix(char *str, const char *suffix)
{
    if (allc_cstr_is_ending_with(str, suffix)) {
        size_t suffix_length = allc_cstr_length(suffix);
        size_t i = allc_cstr_find_cstr(str, -1, suffix);
        memset(str + i, 0, suffix_length);
    }
}

void allc_cstr_remove_prefix(char *str, const char *prefix)
{
    if (allc_cstr_is_starting_with(str, prefix)) {
        size_t prefix_length = allc_cstr_length(prefix);
        allc_cstr_shift_left(str, prefix_length);
    }
}

size_t  allc_cstr_strip_left_blank(char *str)
{
    const char *p = str;
    for (; *p != 0; ++p) {
        if (isblank(*p)) { continue; }
        allc_cstr_remove(str, 0, p - str);
        break;
    }
    return p - str;
}

size_t  allc_cstr_strip_right_blank(char *str)
{
    size_t length = allc_cstr_length(str);
    const char *p = str + length - 1;
    for (; p != str - 1; --p) {
        if (isblank(*p)) { continue; }
        allc_cstr_remove(str, p+1 - str, length);
        break;
    }
    return length - (p+1 - str);
}

size_t  allc_cstr_strip_blank(char *str_buf)
{
    size_t n = allc_cstr_strip_left_blank(str_buf);
    size_t m = allc_cstr_strip_right_blank(str_buf);
    return n + m;
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

// String Representation {{{

const char *allc_cstr_repr_bool(bool b)
{
    return b ? ALLC_CSTR_TRUE : ALLC_CSTR_FALSE;
}

// Other {{{

void allc_cstr_copy(const char *from, char *to)
{
    strcpy(to, from);
}

void allc_cstr_copy_n(const char *from, size_t n, char *to)
{
    strncpy(to, from, n);
}

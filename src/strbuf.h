/*
 *  STB-style header library of a string buffer implementation.
 *
 *  Description
 *  ===========
 *
 *  Provides
 *  ========
 *
 *  StrBuf
 *
 *  Options
 *  =======
 *
 *  ALLC_STRBUF_EXPANDED_TAB
 *
 *  Library
 *  =======
 *
 *  Dependencies:
 *    - allocator.h
 *    - cstr.h
 *
 *  Version: 0.0.0
 *
 *  Author: Kaj Munhoz Arfvidsson, 2023
 */

#ifndef ALLC_STRBUF_GUARD // {{{1
#define ALLC_STRBUF_GUARD

#include "allocator.h"

// Type Declarations {{{2

typedef struct {
    char *data;
    size_t length;          // does not include null terminator
    size_t capacity;        // does include null terminator
    Allocator allocator;
} StrBuf;

// StrBuf management {{{2

/*
 * Create a new string buffer.
 *
 * `length + 1` bytes will be allocated and content is initialized to zero.
 *
 * The string buffer must be freed with `allc_strbuf_del` once it is no longer used.
 */
StrBuf allc_strbuf_new(size_t length, Allocator allocator);

/* Free the allocated memory owned by the string buffer. */
void allc_strbuf_del(StrBuf *str_buf);

/* Create a new string buffer that copies from `str_buf`. */
StrBuf allc_strbuf_copy(StrBuf *str_buf);

/* Create a new string buffer containing a copy of the C-style string `str`. */
StrBuf allc_strbuf_copy_cstr(const char *str, Allocator allocator);

/* Increase the string buffer capacity with `n` bytes. */
void allc_strbuf_grow(StrBuf *str_buf, size_t n);

/* Increase the string buffer capacity to `n` bytes in total, iff current capacity < `n`. */
void allc_strbuf_ensure_capacity(StrBuf *str_buf, size_t n);

// String manipulation {{{2

/* Append a copy of the C-style string `str` to the end of the string buffer. */
void allc_strbuf_append_cstr(StrBuf *str_buf, const char *str);

/*
 * Insert a copy of the C-style string `str` at the index `pos` of the string buffer.
 *
 * If `pos` is negative then insert the string at the index counted from the back.
 * However, if `pos` is outside the length of the string buffer then [TODO]
 *
 * Any existing text starting at `pos` is right shifted by the new strings length using
 * `allc_cstr_rshift`.
 */
void allc_strbuf_insert_cstr(StrBuf *str_buf, size_t pos, const char *str);

/*
 * Remove the substring that starts at `from` and continues until, but not including, `to`.
 *
 * The removal uses `allc_cstr_lshift` on the string starting at `from` for `to-from` bytes.
 *
 * Both `from` and `to` must be inside the string buffer length, otherwise [TODO]
 */
void allc_strbuf_remove(StrBuf *str_buf, size_t from, size_t to);

/* Remove the prefix `str` if it exists. */
void allc_strbuf_remove_prefix(StrBuf *str_buf, const char *str);

/* Remove suffix `str` if it exists. */
void allc_strbuf_remove_suffix(StrBuf *str_buf, const char *str);

#endif // ALLC_STRBUF_GUARD }}}1

#ifdef ALLC_IMPL_STRBUF // {{{1
#ifdef ALLC_IMPL_DEPENDENCIES
#   define ALLC_IMPL_ALLOCATOR
#   define ALLC_IMPL_CSTR
#endif
#ifndef ALLC_IMPL_STRBUF_GUARD
#define ALLC_IMPL_STRBUF_GUARD

#include <string.h>

#include "cstr.h"
#include "allocator.h"

// Macros {{{2

#ifndef ALLC_STRBUF_EXPANDED_TAB
#   define ALLC_STRBUF_EXPANDED_TAB "    "
#endif

// StrBuf management {{{2

StrBuf allc_strbuf_new(size_t length, Allocator allocator)
{
    char *data = allocator.alloc(length + 1);
    memset(data, 0, length + 1);
    StrBuf result = {
        .data = data,
        .length = length,
        .capacity = length + 1,
        .allocator = allocator,
    };
    return result;
}

void allc_strbuf_del(StrBuf *str_buf)
{
    str_buf->allocator.free(str_buf->data);
}

StrBuf allc_strbuf_copy(StrBuf *str_buf)
{
    StrBuf result = allc_strbuf_new(str_buf->length, str_buf->allocator);
    memcpy(result.data, str_buf->data, str_buf->length + 1);
    return result;
}

StrBuf allc_strbuf_copy_cstr(const char *str, Allocator allocator)
{
    size_t length = allc_cstr_length(str);
    StrBuf result = allc_strbuf_new(length, allocator);
    memcpy(result.data, str, length + 1);
    return result;
}

void allc_strbuf_grow(StrBuf *str_buf, size_t n)
{
    size_t new_size = str_buf->capacity + n;
    str_buf->data = str_buf->allocator.realloc(str_buf->data, new_size);
    str_buf->capacity = new_size;
}

void allc_strbuf_ensure_capacity(StrBuf *str_buf, size_t n)
{
    if (str_buf->capacity < n) {
        allc_strbuf_grow(str_buf, n-str_buf->capacity);
    }
}

// String manipulation {{{2

void allc_strbuf_append_cstr(StrBuf *str_buf, const char *str)
{
    size_t length = allc_cstr_length(str);
    allc_strbuf_ensure_capacity(str_buf, str_buf->length + length + 1);
    memcpy(str_buf->data + str_buf->length, str, length + 1); // length + 1 for null terminator
}

void allc_strbuf_insert_cstr(StrBuf *str_buf, size_t pos, const char *str)
{
    size_t length = allc_cstr_length(str);
    allc_strbuf_ensure_capacity(str_buf, str_buf->length + length + 1);
    allc_cstr_rshift(str_buf->data + pos, length);
    memcpy(str_buf->data + pos, str, length);
}

void allc_strbuf_remove(StrBuf *str_buf, size_t from, size_t to)
{
    allc_cstr_lshift(str_buf->data, to-from);
    memset(str_buf->data + to, 0, str_buf->length - to);
}

void allc_strbuf_remove_prefix(StrBuf *str_buf, const char *str)
{
    if (allc_cstr_is_starting_with(str_buf->data, str)) {
        size_t length = allc_cstr_length(str);
        allc_cstr_lshift(str_buf->data, length);
    }
}

void allc_strbuf_remove_suffix(StrBuf *str_buf, const char *str)
{
    if (allc_cstr_is_ending_with(str_buf->data, str)) {
        size_t length = allc_cstr_length(str);
        size_t i = allc_cstr_findn_cstr(str_buf->data, -1, str);
        memset(str_buf->data + i, 0, length);
    }
}

#endif // ALLC_IMPL_STRBUF_GUARD
#endif // ALLC_IMPL_STRBUF }}}1

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

// StrBuf Management {{{2

/*
 * Create a new string buffer.
 *
 * `length + 1` bytes will be allocated and content is initialized to zero.
 *
 * The string buffer must be freed with `allc_strbuf_del` once it is no longer used.
 */
StrBuf allc_strbuf_new(Allocator allocator);

/*
 * Create a new string buffer from C-style string.
 *
 * The string buffer must be freed with `allc_strbuf_del` once it is no longer used.
 */
StrBuf allc_strbuf_new_cstr(Allocator allocator, const char *str);

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

// String Manipulation {{{2

/* Clear string buffer of content. */
void allc_strbuf_clear(StrBuf *self);

/* Set string buffer content to be exactly C-style string `str`. */
void allc_strbuf_set_cstr(StrBuf *self, const char *str);

/* Append a copy of the C-style string `str` to the end of the string buffer. */
void allc_strbuf_append_cstr(StrBuf *str_buf, const char *str);

/*
 * Insert a copy of the C-style string `str` at the index `pos` of the string buffer.
 *
 * If `pos` is negative then insert the string at the index counted from the back.
 * However, if `pos` is outside the length of the string buffer then [TODO]
 *
 * Any existing text starting at `pos` is right shifted by the new strings length using
 * `allc_cstr_shift_right`.
 */
void allc_strbuf_insert_cstr(StrBuf *str_buf, size_t pos, const char *str);

/* Strips string buffer using `cstr_strip`. */
void    allc_strbuf_strip_blank(StrBuf *self);
void    allc_strbuf_strip_left_blank(StrBuf *self);
void    allc_strbuf_strip_right_blank(StrBuf *self);

/* Split string buffer in two (copy operation). */
void    allc_strbuf_split_at(StrBuf *self, size_t i, StrBuf *left, StrBuf *right);

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

StrBuf allc_strbuf_new(Allocator allocator)
{
    char *data = allocator.alloc(1);
    memset(data, 0, 1);
    StrBuf result = {
        .data = data,
        .length = 0,
        .capacity = 1,
        .allocator = allocator,
    };
    return result;
}

void    allc_strbuf_clear(StrBuf *self)
{
    // memset(self->data, 0, self->length);
    self->data[0] = 0;
    self->length = 0;
}

void    allc_strbuf_set_cstr(StrBuf *self, const char *str)
{
    size_t length = allc_cstr_length(str);
    allc_strbuf_ensure_capacity(self, length + 1);
    allc_strbuf_clear(self);
    allc_cstr_copy(str, self->data);
    self->length = length;
}

StrBuf allc_strbuf_new_cstr(Allocator allocator, const char *str)
{
    StrBuf str_buf = allc_strbuf_new(allocator);
    allc_strbuf_append_cstr(&str_buf, str);
    return str_buf;
}

void    allc_strbuf_strip_left_blank(StrBuf *self)
{
    size_t n = allc_cstr_strip_left_blank(self->data);
    self->length -= n;
}

void    allc_strbuf_strip_right_blank(StrBuf *self)
{
    size_t n = allc_cstr_strip_right_blank(self->data);
    self->length -= n;
}

void    allc_strbuf_strip_blank(StrBuf *self)
{
    size_t n = allc_cstr_strip_blank(self->data);
    self->length -= n;
}

void allc_strbuf_del(StrBuf *str_buf)
{
    str_buf->allocator.free(str_buf->data);
}

StrBuf allc_strbuf_copy(StrBuf *str_buf)
{
    StrBuf result = allc_strbuf_new(str_buf->allocator);
    allc_strbuf_ensure_capacity(&result, str_buf->length + 1);
    memcpy(result.data, str_buf->data, str_buf->length + 1);
    return result;
}

StrBuf allc_strbuf_copy_cstr(const char *str, Allocator allocator)
{
    StrBuf result = allc_strbuf_new(allocator);
    allc_strbuf_append_cstr(&result, str);
    return result;
}

void allc_strbuf_grow(StrBuf *str_buf, size_t n)
{
    size_t new_size = str_buf->capacity + n;
    str_buf->data = str_buf->allocator.realloc(str_buf->data, new_size);
    str_buf->capacity = new_size;
}

void allc_strbuf_ensure_capacity(StrBuf *self, size_t n)
{
    if (self->capacity < n) {
        allc_strbuf_grow(self, n - self->capacity);
    }
}

// String manipulation {{{2

void allc_strbuf_append_cstr(StrBuf *str_buf, const char *str)
{
    size_t length = allc_cstr_length(str);
    allc_strbuf_ensure_capacity(str_buf, str_buf->length + length + 1);
    memcpy(str_buf->data + str_buf->length, str, length + 1); // length + 1 for null terminator
    str_buf->length += length;
}

void    allc_strbuf_split_at(StrBuf *self, size_t i, StrBuf *left, StrBuf *right)
{
    if (self == left) {
        allc_strbuf_set_cstr(right, self->data + i);
        self->data[i] = 0;
        left->length = i;
    } else if (self == right) {
        allc_strbuf_ensure_capacity(left, i+1);
        allc_cstr_copy_n(self->data, i, left->data);
        left->length = i;
        allc_cstr_shift_left(self->data, i);
        self->length -= i;
    } else {
        allc_strbuf_set_cstr(right, self->data + i);
        allc_cstr_copy_n(self->data, i, left->data);
        allc_strbuf_set_cstr(right, self->data + i);
    }
}

#endif // ALLC_IMPL_STRBUF_GUARD
#endif // ALLC_IMPL_STRBUF }}}1

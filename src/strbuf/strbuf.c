#include "strbuf.h"
#include "../cstr.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>


// String Buffer {{{1

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

void allc_strbuf_clear(StrBuf *self)
{
    // memset(self->data, 0, self->length);
    self->data[0] = 0;
    self->length = 0;
}

void allc_strbuf_set_fmt(StrBuf *self, const char *fmt, ...)
{
    va_list ap1, ap2;
    va_start(ap1, fmt);
    va_copy(ap2, ap1);
    int n = vsnprintf(NULL, 0, fmt, ap2);
    va_end(ap2);
    allc_strbuf_ensure_capacity(self, n);
    vsprintf(self->data, fmt, ap1);
    va_end(ap1);
    self->length = n - 1;
}

void allc_strbuf_set_cstr(StrBuf *self, const char *str)
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

void allc_strbuf_strip_left_blank(StrBuf *self)
{
    size_t n = allc_cstr_strip_left_blank(self->data);
    self->length -= n;
}

void allc_strbuf_strip_right_blank(StrBuf *self)
{
    size_t n = allc_cstr_strip_right_blank(self->data);
    self->length -= n;
}

void allc_strbuf_strip_blank(StrBuf *self)
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
    result.length = str_buf->length;
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

void allc_strbuf_append_cstr(StrBuf *str_buf, const char *str)
{
    size_t length = allc_cstr_length(str);
    allc_strbuf_ensure_capacity(str_buf, str_buf->length + length + 1);
    memcpy(str_buf->data + str_buf->length, str, length + 1); // length + 1 for null terminator
    str_buf->length += length;
}

void    allc_strbuf_insert_cstr(StrBuf *str_buf, size_t pos, const char *str)
{
    size_t length = allc_cstr_length(str);
    allc_strbuf_ensure_capacity(str_buf, str_buf->length + length + 1);
    allc_cstr_shift_right(str_buf->data + pos, length);
    memcpy(str_buf->data + pos, str, length);
    str_buf->length += length;
}

void allc_strbuf_split_at(StrBuf *self, size_t i, StrBuf *left, StrBuf *right)
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


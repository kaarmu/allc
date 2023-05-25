/**
 *  String buffer.
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
 *  1) String Buffer
 *
 *      [x] StrBuf
 *      [x] allc_strbuf_new
 *      [x] allc_strbuf_new_cstr
 *      [x] allc_strbuf_del
 *      [x] allc_strbuf_copy
 *      [x] allc_strbuf_copy_cstr
 *      [x] allc_strbuf_grow
 *      [x] allc_strbuf_ensure_capacity
 *      [x] allc_strbuf_clear
 *      [x] allc_strbuf_set_fmt
 *      [x] allc_strbuf_set_cstr
 *      [x] allc_strbuf_append_cstr
 *      [x] allc_strbuf_insert_cstr
 *      [x] allc_strbuf_strip_blank
 *      [x] allc_strbuf_strip_left_blank
 *      [x] allc_strbuf_strip_right_blank
 *      [x] allc_strbuf_split_at
 *      [ ] allc_strbuf_justify_center
 *      [ ] allc_strbuf_justify_left
 *      [ ] allc_strbuf_justify_right
 *      [ ] allc_strbuf_replace
 *
 * (Authored by Kaj Munhoz Arfvidsson, 2023)
 **/

#ifndef ALLC_STRBUF__GUARD
#define ALLC_STRBUF__GUARD

// Includes {{{1
// -------------

#include "../allocator/allocator.h"


// String Buffer {{{1
// ------------------

typedef struct StrBuf {
    char *data;
    size_t length;          // does not include null terminator
    size_t capacity;        // does include null terminator
    Allocator allocator;
} StrBuf;

/**
 * Create a new string buffer.
 *
 * `length + 1` bytes will be allocated and content is initialized to zero.
 *
 * The string buffer must be freed with `allc_strbuf_del` once it is no longer used.
 **/
StrBuf allc_strbuf_new(Allocator allocator);

/**
 * Create a new string buffer from C-style string.
 *
 * The string buffer must be freed with `allc_strbuf_del` once it is no longer used.
 **/
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

/* Clear string buffer of content. */
void allc_strbuf_clear(StrBuf *self);

/* Set string buffer content to be result of formatting `fmt`. */
void allc_strbuf_set_fmt(StrBuf *self, const char *fmt, ...);

/* Set string buffer content to be exactly C-style string `str`. */
void allc_strbuf_set_cstr(StrBuf *self, const char *str);

/* Append a copy of the C-style string `str` to the end of the string buffer. */
void allc_strbuf_append_cstr(StrBuf *str_buf, const char *str);

/**
 * Insert a copy of the C-style string `str` at the index `pos` of the string buffer.
 *
 * If `pos` is negative then insert the string at the index counted from the back.
 * However, if `pos` is outside the length of the string buffer then [TODO]
 *
 * Any existing text starting at `pos` is right shifted by the new strings length using
 * `allc_cstr_shift_right`.
 **/
void allc_strbuf_insert_cstr(StrBuf *str_buf, size_t pos, const char *str);

/* Strips string buffer using `cstr_strip`. */
void    allc_strbuf_strip_blank(StrBuf *self);
void    allc_strbuf_strip_left_blank(StrBuf *self);
void    allc_strbuf_strip_right_blank(StrBuf *self);

/* Split string buffer in two (copy operation). */
void    allc_strbuf_split_at(StrBuf *self, size_t i, StrBuf *left, StrBuf *right);

#endif // ALLC_STRBUF__GUARD

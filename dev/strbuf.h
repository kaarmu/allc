/**
 *  allc_string_t buffer.
 *
 *  Description
 *  ===========
 *
 *  Options
 *  =======
 *
 * (Authored by Kaj Munhoz Arfvidsson, 2023)
 **/

// #define ALLC_IMPL // During development - removed to avoid redefinition

#ifndef ALLC_STRBUF__GUARD
#define ALLC_STRBUF__GUARD

// Includes {{{1
// =============

#include "allocator.h"
#include "cstr.h"

#include <sys/types.h>

// Macros {{{1
// ===========

#ifdef ALLC_IMPL
#define ALLC_STRBUF_IMPL
#endif


// StrBuf {{{1
// ===========

struct allc_strbuf_s
{
  allc_allocator_t allocator;
  size_t capacity; // does include null terminator
  size_t length;   // does not include null terminator
  char buf[];
};
typedef struct allc_strbuf_s *StrBuf;

struct allc_strbuf_pair_s
{
  StrBuf left, right;
};

/* Create a new string buffer. */
StrBuf allc_strbuf_new(allc_allocator_t allocator, size_t capacity);

/* Create a new string buffer from C-style string. */
StrBuf allc_strbuf_new_from_cstr(allc_allocator_t allocator, allc_string_t str);

/* Create a new string buffer that copies from `strbuf`. */
StrBuf allc_strbuf_copy(StrBuf *self);

/* Free the allocated memory owned by the string buffer. */
void allc_strbuf_delete(StrBuf self);

// StrBuf - Object Inspection {{{2
// -------------------------------

bool allc_strbuf_is_equal(StrBuf *self, StrBuf *other);

bool allc_strbuf_is_equal_cstr(StrBuf *self, allc_string_t other);

// StrBuf - Object Modifiers {{{2
// ------------------------------

/* Increase the string buffer capacity with `n` bytes. */
void allc_strbuf_grow(StrBuf *self, size_t n);

/**
 * Increase the string buffer capacity to `n` bytes in total, iff current
 * capacity < `n`.
 **/
void allc_strbuf_ensure_capacity(StrBuf *self, size_t n);

/* Clear string buffer of content. */
void allc_strbuf_clear(StrBuf *self);

/* Set string buffer content to be result of formatting `fmt`. */
void allc_strbuf_set_fmt(StrBuf *self, allc_string_t fmt, ...);

/* Set string buffer content to be exactly C-style string `str`. */
void allc_strbuf_set_cstr(StrBuf *self, allc_string_t str);

/* Append a copy of the C-style string `str` to the end of the string buffer. */
void allc_strbuf_append_cstr(StrBuf *strbuf, allc_string_t str);

/**
 * Insert a copy of the C-style string `str` at the index `pos` of the string
 * buffer.
 *
 * If `pos` is negative then insert the string at the index counted from the back. 
 * If `pos` is greater than the length of the string buffer, treat it as if it's equal to the length (i.e., append to the end).
 * If `pos` is a negative number beyond the string's start, treat it as 0 (i.e., insert at the beginning). 
 *
 * Any existing text starting at `pos` is right shifted by the new strings
 * length using `allc_cstr_shift_right`.
 **/
void allc_strbuf_insert_cstr(StrBuf *self, ssize_t pos, allc_string_t str);

/* Strips string buffer using `cstr_strip`. */
void allc_strbuf_strip_blank(StrBuf *self);

void allc_strbuf_strip_left_blank(StrBuf *self);

void allc_strbuf_strip_right_blank(StrBuf *self);

// StrBuf - Object Conversions {{{2
// --------------------------------

/**
 * Split string buffer in two at given index.
 *
 * Two new string buffers will be created containing one part each. The string
 * buffer argument will be deleted.
 **/

struct allc_strbuf_pair_s allc_strbuf_split_at(StrBuf self, size_t i);

/**
 * Split string buffer in two on nth occurence of char.
 *
 * Two new string buffers will be created containing one part each. The string
 * buffer argument will be deleted.
 **/
struct allc_strbuf_pair_s allc_strbuf_split_on_char(StrBuf self, ssize_t n, const char chr);

#endif // ALLC_STRBUF__GUARD

/****************************************************************************/

#ifndef ALLC_STRBUF_IMPL__GUARD
#define ALLC_STRBUF_IMPL__GUARD
#else
#undef ALLC_STRBUF_IMPL
#endif

#ifdef ALLC_STRBUF_IMPL

// Includes {{{1
// -------------

#include <stdarg.h>
#include <stdio.h>
#include <string.h>


// allc_string_t Buffer {{{1
// ------------------

StrBuf allc_strbuf_new(allc_allocator_t allocator, size_t capacity) {
  StrBuf self = allc_allocator_alloc(allocator, sizeof(struct allc_strbuf_s) + capacity);
  *self = (struct allc_strbuf_s){
      .allocator = allocator,
      .capacity = capacity,
      .length = 0,
  };
  // Initialize buffer to zero to ensure null termination
  if (capacity > 0) {
    self->buf[0] = '\0';
  }
  return self;
}

StrBuf allc_strbuf_new_from_cstr(allc_allocator_t allocator, allc_string_t str) {
  StrBuf self = allc_strbuf_new(allocator, 0);
  allc_strbuf_append_cstr(&self, str);
  return self;
}

StrBuf allc_strbuf_copy(StrBuf *self) {
  StrBuf result = allc_strbuf_new((*self)->allocator, (*self)->length + 1);
  memcpy(result->buf, (*self)->buf, (*self)->length + 1);
  result->length = (*self)->length;
  return result;
}

void allc_strbuf_delete(StrBuf self) { allc_allocator_free(self->allocator, self); }

// Path - Object Modifiers {{{2
// ----------------------------

void allc_strbuf_grow(StrBuf *self, size_t n) {
  (*self)->capacity += n;
  *self = allc_allocator_realloc((*self)->allocator, (*self), sizeof(struct allc_strbuf_s) + (*self)->capacity);
}

void allc_strbuf_ensure_capacity(StrBuf *self, size_t n) {
  if ((*self)->capacity < n) {
    allc_strbuf_grow(self, n - (*self)->capacity);
  }
}

void allc_strbuf_clear(StrBuf *self) {
  memset((*self)->buf, 0, (*self)->length);
  (*self)->length = 0;
}

void allc_strbuf_set_fmt(StrBuf *self, allc_string_t fmt, ...) {
  va_list ap1, ap2;
  va_start(ap1, fmt);
  va_copy(ap2, ap1);
  int n = vsnprintf(NULL, 0, fmt, ap2);
  va_end(ap2);
  allc_strbuf_ensure_capacity(self, n + 1);
  vsprintf((*self)->buf, fmt, ap1);
  va_end(ap1);
  (*self)->length = n;
}

void allc_strbuf_set_cstr(StrBuf *self, allc_string_t str) {
  size_t length = allc_cstr_length(str);
  allc_strbuf_ensure_capacity(self, length + 1);
  allc_strbuf_clear(self);
  allc_cstr_copy(str, (*self)->buf);
  (*self)->length = length;
}

void allc_strbuf_append_cstr(StrBuf *self, allc_string_t other) {
  size_t other_length = allc_cstr_length(other);
  allc_strbuf_ensure_capacity(self, (*self)->length + other_length + 1);
  memcpy((*self)->buf + (*self)->length, other,
         other_length + 1); // other_length + 1 for null terminator
  (*self)->length += other_length;
}

void allc_strbuf_insert_cstr(StrBuf *self, ssize_t pos, allc_string_t str) {
  size_t str_length = allc_cstr_length(str);
  size_t buffer_length = (*self)->length;
  size_t insert_pos;
  
  // Calculate the actual insertion position
  if (pos >= 0) {
    insert_pos = (size_t)pos > buffer_length ? buffer_length : (size_t)pos;
  } else {
    ssize_t neg_offset = -pos;
    insert_pos = (size_t)neg_offset > buffer_length ? 0 : buffer_length - (size_t)neg_offset;
  }
  
  allc_strbuf_ensure_capacity(self, (*self)->length + str_length + 1);
  
  // Shift existing content to the right to make room for the new string
  if (insert_pos < buffer_length) {
    memmove((*self)->buf + insert_pos + str_length, 
            (*self)->buf + insert_pos, 
            buffer_length - insert_pos + 1); // +1 for null terminator
  }
  
  // Insert the new string
  memcpy((*self)->buf + insert_pos, str, str_length);
  (*self)->length += str_length;
  
  // Ensure null termination
  (*self)->buf[(*self)->length] = '\0';
}

void allc_strbuf_strip_blank(StrBuf *self) {
  size_t n = allc_cstr_strip_blank((*self)->buf);
  (*self)->length -= n;
}

void allc_strbuf_strip_left_blank(StrBuf *self) {
  size_t n = allc_cstr_strip_left_blank((*self)->buf);
  (*self)->length -= n;
}

void allc_strbuf_strip_right_blank(StrBuf *self) {
  size_t n = allc_cstr_strip_right_blank((*self)->buf);
  (*self)->length -= n;
}

struct allc_strbuf_pair_s allc_strbuf_split_at(StrBuf self, size_t i) {
  struct allc_strbuf_pair_s result;
  result.right = allc_strbuf_new_from_cstr(self->allocator, self->buf + i),
  *(self->buf + i) = '\0';
  result.left = allc_strbuf_new_from_cstr(self->allocator, self->buf),
  allc_strbuf_delete(self);
  return result;
}

struct allc_strbuf_pair_s 
allc_strbuf_split_on_char(StrBuf self, ssize_t n, const char chr) {
  size_t i = allc_cstr_find_char(self->buf, n, chr);
  struct allc_strbuf_pair_s result;
  
  if (i < allc_cstr_length(self->buf)) {
    // Character found, split excluding the delimiter
    result.left = allc_strbuf_new_from_cstr(self->allocator, "");
    allc_strbuf_ensure_capacity(&result.left, i + 1);
    memcpy(result.left->buf, self->buf, i);
    result.left->buf[i] = '\0';
    result.left->length = i;
    
    result.right = allc_strbuf_new_from_cstr(self->allocator, self->buf + i + 1);
  } else {
    // Character not found, left gets the whole string, right is empty
    result.left = allc_strbuf_new_from_cstr(self->allocator, self->buf);
    result.right = allc_strbuf_new_from_cstr(self->allocator, "");
  }
  
  allc_strbuf_delete(self);
  return result;
}

// Path - Boolean Statements {{{2
// ------------------------------

bool allc_strbuf_is_equal(StrBuf *self, StrBuf *other) {
  return allc_cstr_is_equal((*self)->buf, (*other)->buf);
}

bool allc_strbuf_is_equal_cstr(StrBuf *self, allc_string_t other) {
  return allc_cstr_is_equal((*self)->buf, other);
}

// }}}1

#endif // ALLC_STRBUF_IMPL

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker

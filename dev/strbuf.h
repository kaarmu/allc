/**
 *  String buffer.
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
StrBuf allc_strbuf_new_from_cstr(allc_allocator_t allocator, String str);

/* Create a new string buffer that copies from `strbuf`. */
StrBuf allc_strbuf_copy(StrBuf *self);

/* Free the allocated memory owned by the string buffer. */
void allc_strbuf_delete(StrBuf self);

// StrBuf - Object Inspection {{{2
// -------------------------------

bool allc_strbuf_is_equal(StrBuf *self, StrBuf *other);

bool allc_strbuf_is_equal_cstr(StrBuf *self, String other);

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
void allc_strbuf_set_fmt(StrBuf *self, String fmt, ...);

/* Set string buffer content to be exactly C-style string `str`. */
void allc_strbuf_set_cstr(StrBuf *self, String str);

/* Append a copy of the C-style string `str` to the end of the string buffer. */
void allc_strbuf_append_cstr(StrBuf *strbuf, String str);

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
void allc_strbuf_insert_cstr(StrBuf *self, ssize_t pos, String str);

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


// String Buffer {{{1
// ------------------

StrBuf allc_strbuf_new(allc_allocator_t allocator, size_t capacity) {
  StrBuf self = allc_allocator_alloc(allocator, sizeof(struct allc_strbuf_s) + capacity);
  *self = (struct allc_strbuf_s){
      .allocator = allocator,
      .capacity = capacity,
      .length = 0,
  };
  return self;
}

StrBuf allc_strbuf_new_from_cstr(allc_allocator_t allocator, String str) {
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

void allc_strbuf_set_fmt(StrBuf *self, String fmt, ...) {
  va_list ap1, ap2;
  va_start(ap1, fmt);
  va_copy(ap2, ap1);
  int n = vsnprintf(NULL, 0, fmt, ap2);
  va_end(ap2);
  allc_strbuf_ensure_capacity(self, n + 1);
  vsprintf((*self)->buf, fmt, ap1);
  va_end(ap1);
  (*self)->length = n - 1;
}

void allc_strbuf_set_cstr(StrBuf *self, String str) {
  size_t length = allc_cstr_length(str);
  allc_strbuf_ensure_capacity(self, length + 1);
  allc_strbuf_clear(self);
  allc_cstr_copy(str, (*self)->buf);
  (*self)->length = length;
}

void allc_strbuf_append_cstr(StrBuf *self, String other) {
  size_t other_length = allc_cstr_length(other);
  allc_strbuf_ensure_capacity(self, (*self)->length + other_length + 1);
  memcpy((*self)->buf + (*self)->length, other,
         other_length + 1); // other_length + 1 for null terminator
  (*self)->length += other_length;
}

void allc_strbuf_insert_cstr(StrBuf *self, ssize_t pos, String str) {
  size_t i, length = allc_cstr_length(str);
  ssize_t length_signed = (ssize_t) length;
  i = 0 <= pos && pos < length_signed ? (size_t) pos :
      0 <= pos && length_signed <= pos ? length :
      pos < 0 && -length_signed <= pos ? (size_t) (length_signed + pos) : 0; 
  allc_strbuf_ensure_capacity(self, (*self)->length + length + 1);
  allc_cstr_shift_right((*self)->buf + i, length);
  memcpy((*self)->buf + i, str, length);
  (*self)->length += length;
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
  ssize_t i = allc_cstr_find_char(self->buf, n, chr);
  return allc_strbuf_split_at(self, i);
}

// Path - Boolean Statements {{{2
// ------------------------------

bool allc_strbuf_is_equal(StrBuf *self, StrBuf *other) {
  return allc_cstr_is_equal((*self)->buf, (*other)->buf);
}

bool allc_strbuf_is_equal_cstr(StrBuf *self, String other) {
  return allc_cstr_is_equal((*self)->buf, other);
}

// }}}1

#endif // ALLC_STRBUF_IMPL

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker

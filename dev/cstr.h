/***

C-style strings
===============

Description
-----------

Options
-------

Authored by Kaj Munhoz Arfvidsson, 2023.

***/

#ifndef ALLC_CSTR_GUARD
#define ALLC_CSTR_GUARD

// Includes {{{1
// =============

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>


// Macros {{{1
// ===========

#ifdef ALLC_IMPL
#define ALLC_CSTR_IMPL
#endif


// String {{{1
// ===========

typedef const char *String;

typedef char *CharArray;

/* Copy entire C-style string `from` to `to`. */
void allc_cstr_copy(String from, CharArray to);

/* Copy at most `n` characters from C-style string `from` to `to`. */
void allc_cstr_copy_n(String from, size_t n, CharArray to);


// String - Inspection {{{2
// ------------------------

/* Test that string contains only space or tab characters. */
bool allc_cstr_is_blank(String str);

/**
 * Test that string contains only characters in '0'..'9'.
 *
 * Returns false on empty string
 **/
bool allc_cstr_is_digit(String str);

/**
 * Test that string contains only characters in 'A'..'Z'.
 *
 * Returns false on empty string
 **/
bool allc_cstr_is_upper(String str);

/* Test that string contains only characters in 'a'..'z'. */
bool allc_cstr_is_lower(String str);

/* Test that string contains only characters in 'A'..'Z' or 'a'..'z'. */
bool allc_cstr_is_alpha(String str);

/**
 * Test that string contains only characters in '0'..'9', 'A'..'Z' or
 * 'a'..'z'.
 **/
bool allc_cstr_is_alnum(String str);

/**
 * Test that string contains only characters '0' or '1'.
 *
 * Returns false on empty string
 **/
bool allc_cstr_is_bin(String str);

/**
 * Test that string contains only characters in '0'..'7'.
 *
 * Returns false on empty string
 **/
bool allc_cstr_is_octo(String str);

/**
 * Test that string contains only characters in '0'..'9', 'a'..'f' or 'A'..'F'.
 *
 * Returns false on empty string
 **/
bool allc_cstr_is_hexa(String str);

/* Test that the two strings are equal. */
bool allc_cstr_is_equal(String str1, String str2);

/**
 * Test that string starts with substring.
 *
 * If substring is empty, return true.
 **/
bool allc_cstr_is_starting_with(String str, String sub);

/**
 * Test that string starts with substring.
 *
 * If substring is empty, return true.
 **/
bool allc_cstr_is_ending_with(String str, String sub);

/**
 * Test that string contains substring.
 **/
bool allc_cstr_is_containing(String str, String sub);

/**
 * Test that string is empty.
 *
 * A string is empty when the first character is '\0'.
 **/
bool allc_cstr_is_empty(String str);

/**
 * Return length of string.
 *
 * Length does not include the null terminator.
 **/
size_t allc_cstr_length(String str);

/**
 * Find the index of the first occurrence of a blank character in string.
 *
 * If a blank character cannot be found, the string length is returned.
 **/
size_t allc_cstr_find_blank(String str);

/**
 * Find the index of the `n`th occurrence of character `c` in string.
 *
 * If `n` is zero, return 0.
 * If the character cannot be found, return the string length.
 **/
size_t allc_cstr_find_char(String str, ssize_t n, const char chr);

/**
 * Find the index of the `n`th occurence of substring `sub` in string.
 *
 * If `n` is zero, return 0.
 * If the character cannot be found, the string length is returned.
 **/
size_t allc_cstr_find_cstr(String str, ssize_t n, String sub);

// String - Manipulation {{{2
// --------------------------

/**
 * Remove chracters between index `from` up to, but not including, index
 * `to`.
 **/
void allc_cstr_remove(CharArray str, size_t from, size_t to);

/* Remove a known suffix. */
void allc_cstr_remove_suffix(CharArray str, String suffix);

/* Remove a known prefix. */
void allc_cstr_remove_prefix(CharArray str, String prefix);

/**
 * Remove all blank characters that begin the string.
 *
 * Returns the number of characters that were removed.
 **/
size_t allc_cstr_strip_left_blank(CharArray str);

/**
 * Remove all blank characters that end the string.
 *
 * Returns the number of characters that were removed.
 **/
size_t allc_cstr_strip_right_blank(CharArray str);

/**
 * Remove all blank characters at both beginning and end of the string.
 *
 * Returns the number of characters that were removed.
 **/
size_t allc_cstr_strip_blank(CharArray str_buf);

/**
 * Left shift string.
 *
 * If `n` is zero, do nothing.
 * If `n` is greater than string length, remove everything.
 **/
void allc_cstr_shift_left(CharArray str, size_t n);

/**
 * Right shift string.
 *
 * If `n` is zero, do nothing.
 * If `n` is greater than string length, remove everything.
 **/
void allc_cstr_shift_right(CharArray str, size_t n);

/**
 * Replace the first `n` characters matching `chr` in a string.
 *
 * If the character cannot be found, do nothing.
 **/
void allc_cstr_replace_char(CharArray str, size_t n, const char chr,
                            const char rpl);

/**
 * Replace all characters matching `chr` in a string.
 *
 * If the character cannot be found, do nothing.
 **/
void allc_cstr_replace_all_char(CharArray str, const char chr, const char rpl);

/**
 * Replace the first `n` substrings matching `sub` in a string.
 *
 * If the new substring is larger than the old, do nothing.
 * If the old substring is larger than the new, replace as much as possible.
 * If the substring cannot be found, do nothing.
 **/
void allc_cstr_replace_cstr(CharArray str, size_t n, String sub, String rpl);

/**
 * Replace all substrings matching `sub` in a string.
 *
 * If the new substring is larger than the old, do nothing.
 * If the old substring is larger than the new, replace as much as possible.
 * If the substring cannot be found, do nothing.
 **/
void allc_cstr_replace_all_cstr(CharArray str, String sub, String rpl);

// String - Case Conversion {{{2
// -----------------------------

/**
 * Capitalize the first character of the string.
 *
 * If the string is empty, do nothing.
 **/
void allc_cstr_capitalize(CharArray str);

/**
 * Capitalize the first character of each word in the string.
 *
 * A word is defined as a sequence of non-blank characters.
 **/
void allc_cstr_capitalize_all(CharArray str);

/**
 * Convert the first character of the string to lowercase.
 *
 * If the string is empty, do nothing.
 **/
void allc_cstr_lower(CharArray str);

/**
 * Convert all characters in the string to lowercase.
 **/
void allc_cstr_lower_all(CharArray str);

/**
 * Convert the first character of the string to uppercase.
 *
 * If the string is empty, do nothing.
 **/
void allc_cstr_upper(CharArray str);

/**
 * Convert all characters in the string to uppercase.
 **/
void allc_cstr_upper_all(CharArray str);

/**
 * Swap the case of the first character in the string.
 *
 * If the string is empty, do nothing.
 **/
void allc_cstr_swap_case(CharArray str);

/**
 * Swap the case of all characters in the string.
 **/
void allc_cstr_swap_case_all(CharArray str);

// String - Representations {{{2
// -----------------------------

String allc_cstr_repr_bool(bool b);

// }}}1

#endif // ALLC_CSTR_GUARD

/****************************************************************************/

#ifndef ALLC_CSTR_IMPL__GUARD
#define ALLC_CSTR_IMPL__GUARD
#else
#undef ALLC_CSTR_IMPL
#endif


#ifdef ALLC_CSTR_IMPL

// String Inspection {{{1
// ======================

bool allc_cstr_is_blank(String str)
{
  // Empty string is not blank according to tests
  if (*str == '\0') return false;
  
  for (String p = str; *p != 0; ++p)
  {
    switch (*p)
    {
    case ' ':
    case '\t':
      continue;
    default:
      return false;
    }
  }
  return true;
}

bool allc_cstr_is_digit(String str)
{
  bool result = *str != 0;
  for (String p = str; *p != 0; ++p)
  {
    char chr = *p;
    result = result && (('0' <= chr && chr <= '9'));
  }
  return result;
}

bool allc_cstr_is_upper(String str)
{
  bool result = *str != 0;
  for (String p = str; *p != 0; ++p)
  {
    char chr = *p;
    result = result && (('A' <= chr && chr <= 'Z'));
  }
  return result;
}

bool allc_cstr_is_lower(String str)
{
  // Empty string returns false according to tests
  if (*str == '\0') return false;
  
  for (String p = str; *p != 0; ++p)
  {
    char chr = *p;
    if (!(chr >= 'a' && chr <= 'z'))
      return false;
  }
  return true;
}

bool allc_cstr_is_alpha(String str)
{
  // Empty string returns false according to tests
  if (*str == '\0') return false;
  
  for (String p = str; *p != 0; ++p)
  {
    char chr = *p;
    if (!((chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z')))
      return false;
  }
  return true;
}

bool allc_cstr_is_alnum(String str)
{
  // Empty string returns false according to tests
  if (*str == '\0') return false;
  
  for (String p = str; *p != 0; ++p)
  {
    char chr = *p;
    if (!((chr >= '0' && chr <= '9') || (chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z')))
      return false;
  }
  return true;
}

bool allc_cstr_is_bin(String str)
{
  bool result = *str != 0;
  for (String p = str; *p != 0; ++p)
  {
    char chr = *p;
    result = result && ((chr == '0' || chr == '1'));
  }
  return result;
}

bool allc_cstr_is_octo(String str)
{
  bool result = *str != 0;
  for (String p = str; *p != 0; ++p)
  {
    char chr = *p;
    result = result && (('0' <= chr && chr <= '7'));
  }
  return result;
}

bool allc_cstr_is_hexa(String str)
{
  bool result = *str != 0;
  for (String p = str; *p != 0; ++p)
  {
    char chr = *p;
    result =
        result && (('0' <= chr && chr <= '9') || ('A' <= chr && chr <= 'F') ||
                   ('a' <= chr && chr <= 'f'));
  }
  return result;
}

bool allc_cstr_is_equal(String str1, String str2)
{
  String p = str1, q = str2;
  for (; *p != 0 && *q != 0; ++p, ++q)
  {
    if (*p != *q)
      return false;
  }
  return *p == *q;
}

bool allc_cstr_is_starting_with(String str, String sub)
{
  String p = str, q = sub;
  for (; *p != 0 && *q != 0; ++p, ++q)
  {
    if (*p != *q)
      return false;
  }
  return true;
}

bool allc_cstr_is_ending_with(String str, String sub)
{
  size_t str_length = allc_cstr_length(str), sub_length = allc_cstr_length(sub);
  String p = str + str_length - sub_length;
  return allc_cstr_is_equal(p, sub);
}

bool allc_cstr_is_containing(String str, String sub)
{
  size_t str_length = allc_cstr_length(str), sub_length = allc_cstr_length(sub);
  if (sub_length == 0)
    return true;
  for (size_t n = 0; n < str_length - sub_length + 1; ++n)
    if (allc_cstr_is_starting_with(str + n, sub))
      return true;
  return false;
}

bool allc_cstr_is_empty(String str) { return *str == '\0'; }

size_t allc_cstr_length(String str)
{
  String p = str;
  for (; *p != 0; ++p)
    ;
  return (size_t)(p - str);
}

size_t allc_cstr_find_blank(String str)
{
  String p = str;
  while (*p != 0 && !isblank(*p))
  {
    ++p;
  }
  return (size_t)(p - str);
}

size_t allc_cstr_find_char(String str, ssize_t n, const char chr)
{
  String p = str;
  if (n == 0)
  {
    return 0;
  }
  else if (0 < n)
  {
    do
    {
      for (; *p != 0 && *p != chr; ++p)
        ;
    } while (--n > 0 && *p != 0 && *(++p) != 0);
  }
  else
  {
    for (; *p != 0; ++p)
      ;
    do
    {
      --p;
      for (; p != str && *p != chr; --p)
        ;
    } while (++n < 0 && p != str);
  }
  return p - str;
}

size_t allc_cstr_find_cstr(String str, ssize_t n, String sub)
{
  size_t length = allc_cstr_length(str);
  size_t sub_length = allc_cstr_length(sub);
  
  if (n == 0) return 0;
  if (sub_length == 0) return length;
  if (sub_length > length) return length;
  
  if (n > 0) {
    // Search forward
    for (size_t i = 0; i <= length - sub_length; i++) {
      if (allc_cstr_is_starting_with(str + i, sub)) {
        n--;
        if (n == 0) return i;
      }
    }
  } else {
    // Search backward
    for (size_t i = length - sub_length + 1; i > 0; i--) {
      if (allc_cstr_is_starting_with(str + i - 1, sub)) {
        n++;
        if (n == 0) return i - 1;
      }
    }
  }
  
  return length;
}

// String Manipulation {{{1
// ------------------------

void allc_cstr_remove(CharArray str, size_t from, size_t to)
{
  if (from >= to) return; // Do nothing if from >= to
  allc_cstr_shift_left(str + from, to - from);
}

void allc_cstr_remove_suffix(CharArray str, String suffix)
{
  if (allc_cstr_is_ending_with(str, suffix))
  {
    size_t suffix_length = allc_cstr_length(suffix);
    size_t i = allc_cstr_find_cstr(str, -1, suffix);
    memset(str + i, 0, suffix_length);
  }
}

void allc_cstr_remove_prefix(CharArray str, String prefix)
{
  if (allc_cstr_is_starting_with(str, prefix))
  {
    size_t prefix_length = allc_cstr_length(prefix);
    allc_cstr_shift_left(str, prefix_length);
  }
}

size_t allc_cstr_strip_left_blank(CharArray str)
{
  String p = str;
  for (; *p != 0; ++p)
  {
    if (isblank(*p))
    {
      continue;
    }
    allc_cstr_remove(str, 0, p - str);
    return p - str;
  }
  // If we reach here, the string contains only blanks
  if (p > str) {
    allc_cstr_remove(str, 0, p - str);
  }
  return p - str;
}

size_t allc_cstr_strip_right_blank(CharArray str)
{
  size_t length = allc_cstr_length(str);
  if (length == 0) return 0;
  
  String p = str + length - 1;
  for (; p >= str; --p)
  {
    if (isblank(*p))
    {
      continue;
    }
    allc_cstr_remove(str, p + 1 - str, length);
    return length - (p + 1 - str);
  }
  // If we reach here, the string contains only blanks
  if (length > 0) {
    allc_cstr_remove(str, 0, length);
  }
  return length;
}

size_t allc_cstr_strip_blank(CharArray str_buf)
{
  size_t n = allc_cstr_strip_left_blank(str_buf);
  size_t m = allc_cstr_strip_right_blank(str_buf);
  return n + m;
}

void allc_cstr_shift_left(CharArray str, size_t n)
{
  size_t length = allc_cstr_length(str);
  CharArray p = str;
  for (; p + n < str + length; ++p)
  {
    *p = *(p + n);
  }
  if (p < str + length)
    memset(p, 0, str + length - p);
}

void allc_cstr_shift_right(CharArray str, size_t n)
{
  size_t length = allc_cstr_length(str);
  CharArray end = str + length;
  for (CharArray p = end - n; p < end; ++p)
  {
    *p = 0;
  }
}

void allc_cstr_replace_char(CharArray str, size_t n, const char chr, const char rpl)
{
  size_t i = 0;
  while (n != 0)
  {
    i = allc_cstr_find_char(str, 1, chr);
    if (str[i] == 0)
      return; // if chr not found then i = length wich implies str[i] = 0.
    str[i] = rpl;
    n -= 1;
    i += 1;
  }
}

void allc_cstr_replace_all_char(CharArray str, const char chr, const char rpl)
{
  size_t i = 0;
  while (true)
  {
    i += allc_cstr_find_char(str + i, 1, chr);
    if (str[i] == 0)
      return; // if chr not found then i = length wich implies str[i] = 0.
    str[i] = rpl;
    i += 1;
  }
}

void allc_cstr_replace_cstr(CharArray str, size_t n, String sub, String rpl)
{
  size_t i = 0,
         sub_length = allc_cstr_length(sub),
         rpl_length = allc_cstr_length(rpl);
  
  // If the new substring is larger than the old, do nothing
  if (rpl_length > sub_length)
    return;
    
  while (n != 0)
  {
    i = allc_cstr_find_cstr(str, 1, sub);
    if (str[i] == 0)
      return; // if sub not found then i = length wich implies str[i] = 0.
    allc_cstr_copy_n(rpl, rpl_length, str + i);
    allc_cstr_shift_left(str + i + rpl_length, sub_length - rpl_length);
    n -= 1;
    i += rpl_length;
  }
}

void allc_cstr_replace_all_cstr(CharArray str, String sub, String rpl)
{
  size_t i = 0, 
         sub_length = allc_cstr_length(sub),
         rpl_length = allc_cstr_length(rpl);
  
  // If the new substring is larger than the old, do nothing
  if (rpl_length > sub_length)
    return;
    
  while (true)
  {
    i += allc_cstr_find_cstr(str + i, 1, sub);
    if (str[i] == 0)
      return; // if sub not found then i = length wich implies str[i] = 0.
    allc_cstr_copy_n(rpl, rpl_length, str + i);
    allc_cstr_shift_left(str + i + rpl_length, sub_length - rpl_length);
    i += rpl_length;
  }
}

// String Representation {{{1
// --------------------------

String allc_cstr_repr_bool(bool b) { return b ? "true" : "false"; }

String allc_cstr_repr_test_result(bool b)
{
  return b ? "passed" : "failed";
}

// String Case Conversion {{{1
// ---------------------------

void allc_cstr_capitalize(CharArray str)
{
  if (*str != '\0' && isalpha(*str))
  {
    *str = toupper(*str);
  }
}

void allc_cstr_capitalize_all(CharArray str)
{
  bool at_word_start = true;
  for (CharArray p = str; *p != '\0'; ++p)
  {
    if (isblank(*p))
    {
      at_word_start = true;
    }
    else if (at_word_start && isalpha(*p))
    {
      *p = toupper(*p);
      at_word_start = false;
    }
    else
    {
      at_word_start = false;
    }
  }
}

void allc_cstr_lower(CharArray str)
{
  if (*str != '\0' && isalpha(*str))
  {
    *str = tolower(*str);
  }
}

void allc_cstr_lower_all(CharArray str)
{
  for (CharArray p = str; *p != '\0'; ++p)
  {
    if (isalpha(*p))
    {
      *p = tolower(*p);
    }
  }
}

void allc_cstr_upper(CharArray str)
{
  if (*str != '\0' && isalpha(*str))
  {
    *str = toupper(*str);
  }
}

void allc_cstr_upper_all(CharArray str)
{
  for (CharArray p = str; *p != '\0'; ++p)
  {
    if (isalpha(*p))
    {
      *p = toupper(*p);
    }
  }
}

void allc_cstr_swap_case(CharArray str)
{
  if (*str != '\0' && isalpha(*str))
  {
    *str = islower(*str) ? toupper(*str) : tolower(*str);
  }
}

void allc_cstr_swap_case_all(CharArray str)
{
  for (CharArray p = str; *p != '\0'; ++p)
  {
    if (isalpha(*p))
    {
      *p = islower(*p) ? toupper(*p) : tolower(*p);
    }
  }
}

// Other {{{1
// ----------

void allc_cstr_copy(String from, CharArray to) { strcpy(to, from); }

void allc_cstr_copy_n(String from, size_t n, CharArray to)
{
  strncpy(to, from, n);
}

// }}}1

#endif // ALLC_CSTR_IMPL

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker

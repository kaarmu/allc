// Includes {{{1
// -------------

#include "cstr.h"


// Regex function {{{1
// -------------------

bool allc_regex_match(String pat, String str) {
  if (pat[0] == '^')
    return rob_regex_match_immediate(pat + 1, str);
  do {
    if (rob_regex_match_immediate(pat, str))
      return true;
  } while (*str++ != '\0');
  return false;
}


// Private Declarations {{{1
// -------------------------

bool rob_regex_match_immediate(String pat, String str) {
  if (pat[0] == '\0')
    return 1;
  if (pat[1] == '*')
    return rob_regex_match_many(pat + 2, pat[0], str);
  if (pat[0] == '$' && pat[1] == '\0')
    return *str == '\0';
  if (*str != '\0' && (pat[0] == '.' || pat[0] == *str))
    return rob_regex_match_immediate(pat + 1, str + 1);
  return 0;
}

bool rob_regex_match_many(String pat, size_t c, String str) {
  do {
    if (rob_regex_match_immediate(pat, str))
      return true;
  } while (*str != '\0' && (*str++ == c || c == '.'));
  return false;
}

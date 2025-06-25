/***
  Simple regular expression matching.

  Description
  ===========

  Rob Pike's regex code is described in:
  http://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html


  Authored by Kaj Munhoz Arfvidsson, 2023.
***/

#ifndef ALLC_REGEX__GUARD
#define ALLC_REGEX__GUARD

// Includes {{{1
// -------------

#include "cstr.h"


// Regex function {{{1
// -------------------

bool allc_regex_match(String pat, String str);

#endif // ALLC_REGEX__GUARD

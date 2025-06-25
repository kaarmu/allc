/***

Path object
===========

Description
-----------

Options
-------

ALLC_PATH_SEP (default="/")

Authored by Kaj Munhoz Arfvidsson, 2023.

***/

#define ALLC_IMPL // During development

#ifdef ALLC_IMPL
#   define ALLC_PATH_IMPL
#endif

#ifndef ALLC_PATH__GUARD
#define ALLC_PATH__GUARD

// Includes {{{1
// =============

#include "cstr.h"
#include "strbuf.h"

#include <stdbool.h>

// Macros {{{1
// ===========

#ifdef ALLC_IMPL
#define ALLC_PATH_IMPL
#endif

#ifndef ALLC_PATH_SEP
#define ALLC_PATH_SEP "/"
#endif

// Path {{{1
// =========

#ifdef ALLC_PATH_IMPL
typedef StrBuf Path;
#else
typedef void *Path;
#endif

// Path - Constructing and Desctructing {{{2
// -----------------------------------------

/**
 * Create a new path object. By default the path is the current directory,
 * ".".
 **/
Path allc_path_new(allc_allocator_t allocator);

Path allc_path_new_from_cwd(allc_allocator_t allocator);

Path allc_path_new_from_home(allc_allocator_t allocator);

Path allc_path_new_from_cstr(allc_allocator_t allocator, String str);

Path allc_path_copy(Path *self);

void allc_path_delete(Path self);

// Path - Object Modifiers {{{2
// ----------------------------

void allc_path_set_cstr(Path *self, String str);

void allc_path_append_part_cstr_1(Path *self, String str);

void allc_path_append_part_cstr_n(Path *self, size_t n, ...);

void allc_path_extend_path(Path *self, Path *other);

// Path - Object Conversions {{{2
// ------------------------------

StrBuf allc_path_to_strbuf(Path self);

Path allc_path_to_canonical_path(Path self);

Path allc_path_to_absolute_path(Path self);

Path allc_path_to_dirname_path(Path self);

StrBuf allc_path_to_basename_strbuf(Path self); // TODO

StrBuf allc_path_to_suffix_strbuf(Path self); // TODO

// Path - Boolean Statements {{{2
// ------------------------------

bool allc_path_is_equal(Path *self, Path *other);

bool allc_path_is_absolute(Path *self);

// Path - Object inspection {{{2
// -----------------------------

size_t allc_path_count_parts(Path *self);

#endif // ALLC_PATH__GUARD

/****************************************************************************/

#ifndef ALLC_PATH_IMPL__GUARD
#define ALLC_PATH_IMPL__GUARD
#else
#undef ALLC_PATH_IMPL
#endif

#ifdef ALLC_PATH_IMPL

// Includes {{{1
// =============

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

// Macros {{{1
// ============

#define return_(value)  \
    do                  \
    {                   \
        result = value; \
        goto defer;     \
    } while (0)

// Path {{{1
// =========

// Path - Constructing and Destructing {{{2
// ----------------------------------------

Path allc_path_new(allc_allocator_t allocator)
{
    return allc_path_new_from_cstr(allocator, ".");
}

Path allc_path_new_from_cwd(allc_allocator_t allocator)
{
    StrBuf strbuf = allc_strbuf_new(allocator, 16);
    char *c = NULL;
    do
    {
        allc_strbuf_ensure_capacity(&strbuf, strbuf->capacity * 2);
        c = getcwd(strbuf->buf, strbuf->capacity);
        strbuf->length = allc_cstr_length(strbuf->buf);
    } while (c == NULL && errno == ERANGE);
    if (c == NULL)
    {
        // TODO: Some other error
    }
    return strbuf;
}

Path allc_path_new_from_home(allc_allocator_t allocator)
{
    Path path = allc_path_new(allocator);
    char *c = getenv("HOME");
    if (c == NULL)
    {
        // TODO: Error when missing HOME
    }
    allc_path_set_cstr(&path, c);
    return path;
}

Path allc_path_new_from_cstr(allc_allocator_t allocator, String str)
{
    return allc_strbuf_new_from_cstr(allocator, str);
}

Path allc_path_copy(Path *self) { return allc_strbuf_copy(self); }

void allc_path_delete(Path self) { allc_strbuf_delete(self); }

// Path - Object Modifiers {{{2
// ----------------------------

void allc_path_set_cstr(Path *self, String str)
{
    return allc_strbuf_set_cstr(self, str);
}

void allc_path_append_part_cstr_1(Path *self, String str)
{
    if ((*self)->length != 0 && !allc_cstr_is_ending_with((*self)->buf, ALLC_PATH_SEP))
    {
        allc_strbuf_append_cstr(self, ALLC_PATH_SEP);
    }
    allc_strbuf_append_cstr(self, str);
}

void allc_path_append_part_cstr_n(Path *self, size_t n, ...)
{
    const char *str;
    va_list ap;
    va_start(ap, n);
    do
    {
        str = va_arg(ap, char *);
        allc_path_append_part_cstr_1(self, str);
        n -= 1;
    } while (0 < n);
    va_end(ap);
}

void allc_path_extend_path(Path *self, Path *other)
{
    if (allc_path_is_absolute(self) && allc_path_is_absolute(other))
    {
        return;
    }
    allc_path_append_part_cstr_1(self, (*other)->buf);
}

// Path - Object Conversions {{{2
// ------------------------------

StrBuf allc_path_to_strbuf(Path self) { return self; }

Path allc_path_to_canonical_path(Path self)
{
    // If there are no path seperations
    if (allc_path_count_parts(&self) <= 1)
    {
        return self;
    }

    // "./" in the beginning
    if (allc_cstr_is_starting_with(self->buf, "." ALLC_PATH_SEP))
    {
        allc_cstr_shift_left(self->buf, 2);
        self->length -= 2;
    }

    // Start at first path separation
    size_t i = allc_cstr_find_cstr(self->buf, 1, ALLC_PATH_SEP);
    while (i < self->length)
    {
        // Case "//"
        if (allc_cstr_is_starting_with(self->buf + i, ALLC_PATH_SEP ALLC_PATH_SEP))
        {
            allc_cstr_shift_left(self->buf + i, 1);
            self->length -= 1;
        }
        // Case "/./"
        else if (allc_cstr_is_starting_with(self->buf + i, ALLC_PATH_SEP "." ALLC_PATH_SEP))
        {
            allc_cstr_shift_left(self->buf + i, 2);
            self->length -= 2;
        }
        // Case "/.."
        else if (allc_cstr_is_starting_with(self->buf + i, ALLC_PATH_SEP ".."))
        {
            for (size_t j = 1; j < i; ++j)
            {
                // "/abcdef/.."
                //  ^i-j   ^i
                if (allc_cstr_is_starting_with(self->buf + i - j, ALLC_PATH_SEP))
                {
                    allc_cstr_shift_left(self->buf + i - j, j + 3);
                    self->length -= j + 2;
                    break;
                }
            }
        }
        i++;
    }

    // "/." in the end
    if (allc_cstr_is_starting_with(self->buf + i - 2, ALLC_PATH_SEP "."))
    {
        allc_cstr_shift_left(self->buf + i - 2, 2);
    }

    return self;
}

Path allc_path_to_absolute_path(Path self)
{
    if (allc_path_is_absolute(&self))
    {
        return self;
    }
    Path new = allc_path_new_from_cwd(self->allocator);
    allc_path_extend_path(&new, &self);
    allc_path_delete(self);
    return allc_path_to_canonical_path(new);
}

Path allc_path_to_dirname_path(Path self)
{
    if (allc_cstr_is_equal(self->buf, ALLC_PATH_SEP))
    {
        return self;
    }
    else if (allc_path_count_parts(&self) <= 1)
    {
        allc_path_set_cstr(&self, ".");
        return self;
    }
    else
    {

        size_t i = allc_cstr_find_cstr(self->buf, -1, ALLC_PATH_SEP);
        allc_cstr_shift_left(self->buf + i, self->length - i);
        self->length = i;
        return self;
    }
}

// Path - Boolean Statements {{{2
// ------------------------------

bool allc_path_is_equal(Path *self, Path *other)
{
    Path p1 = allc_path_copy(self), p2 = allc_path_copy(other);
    p1 = allc_path_to_absolute_path(p1);
    p2 = allc_path_to_absolute_path(p2);
    bool result = allc_cstr_is_equal(p1->buf, p2->buf);
    allc_path_delete(p1);
    allc_path_delete(p2);
    return result;
}

bool allc_path_is_absolute(Path *self)
{
    return allc_cstr_is_starting_with((*self)->buf, ALLC_PATH_SEP);
}

// Path - Object inspection {{{2
// -----------------------------

size_t allc_path_count_parts(Path *self)
{
    size_t i = 0, depth = 0;
    StrBuf strbuf = *self;
    if (strbuf->length == 0)
    {
        return 0;
    }
    if (allc_path_is_absolute(self))
    {
        do
        {
            i++;
        } while (i < strbuf->length && allc_cstr_is_starting_with(strbuf->buf + i, ALLC_PATH_SEP));
    }
    for (; i < strbuf->length; i++)
    {
        if (!allc_cstr_is_starting_with(strbuf->buf + i, ALLC_PATH_SEP))
        {
            continue;
        }
        do
        {
            i++;
        } while (i < strbuf->length && allc_cstr_is_starting_with(strbuf->buf + i, ALLC_PATH_SEP));
        depth++;
    }
    if (allc_cstr_is_ending_with(strbuf->buf, ALLC_PATH_SEP))
    {
        depth--;
    }
    return depth + 1;
}

// }}}1

#endif // ALLC_PATH_IMPL

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker

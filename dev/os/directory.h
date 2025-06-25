/*
 *  Directory Handling.
 *
 *  Description
 *  ===========
 *  This module provides an abstraction layer for directory management using 
 *  platform-specific directory functions. It encapsulates operations such as 
 *  opening, reading, and closing directories in a more structured interface.
 *
 *  Options
 *  =======
 *  Compilation options that control the behavior of the directory module.
 *
 *  Content
 *  =======
 *  Types and subroutines provided by this module for directory handling.
 *
 *  Directory Object
 *  ----------------
 *  [x] allc_directory_t
 *  [x] allc_directory_new
 *  [x] allc_directory_new_opened
 *  [x] allc_directory_delete
 *
 *  Directory Object Accessors
 *  --------------------------
 *  [x] allc_directory_get_status
 *
 *  Directory Object Modifiers
 *  --------------------------
 *  [x] allc_directory_open
 *  [x] allc_directory_close
 *  [x] allc_directory_read
 *  [x] allc_directory_rewind
 */

// Module Dependencies {{{1
// ========================

#define ALLC_IMPL // During development

#ifdef ALLC_IMPL
#   define ALLC_DIRECTORY_IMPL
#endif

#include "../allocator.h"
#include "../path.h"

#include <dirent.h> // for opendir, readdir, closedir
#include <errno.h>

// Macros {{{1
// ===========

#define returnd(value)  \
    do {                \
        result = value; \
        goto defer;     \
    } while (0)

// Declarations {{{1
// =================

#ifndef ALLC_DIRECTORY__GUARD
#define ALLC_DIRECTORY__GUARD

// Directory {{{2
// --------------

typedef enum allc_directory_status_e {
    ALLC_DIRECTORY_SUCCESS,
    ALLC_DIRECTORY_END_OF_DIRECTORY,
    ALLC_DIRECTORY_ERROR_BAD_STATE,
    ALLC_DIRECTORY_ERROR_OPEN_FAILED,
    ALLC_DIRECTORY_ERROR_CLOSE_FAILED,
    ALLC_DIRECTORY_ERROR_READ_FAILED,
} allc_directory_status_e;

#ifdef ALLC_DIRECTORY_IMPL
typedef struct allc_directory_s {
    allc_allocator_t allocator;
    Path path;
    bool open;
    allc_directory_status_e status;
    DIR *handle;
    struct dirent *entry;
} *allc_directory_t;
#else
typedef void *allc_directory_t;
#endif // ALLC_DIRECTORY_IMPL

allc_directory_t allc_directory_new(allc_allocator_t allocator, String path);
allc_directory_t allc_directory_new_opened(allc_allocator_t allocator, String path);
void allc_directory_delete(allc_directory_t self);

// Directory - Object Accessors {{{3

allc_directory_status_e allc_directory_get_status(allc_directory_t *self);

// Directory - Object Modifiers {{{3

allc_directory_status_e allc_directory_open(allc_directory_t *self);
allc_directory_status_e allc_directory_close(allc_directory_t *self);
allc_directory_status_e allc_directory_read(allc_directory_t *self);
allc_directory_status_e allc_directory_rewind(allc_directory_t *self);

#endif // ALLC_DIRECTORY__GUARD
// }}}1

/****************************************************************************/

// Defintions {{{1
// ===============

#ifndef ALLC_DIRECTORY_IMPL__GUARD
#   define ALLC_DIRECTORY_IMPL__GUARD
#else
#   undef ALLC_DIRECTORY_IMPL
#endif

#ifdef ALLC_DIRECTORY_IMPL

// Directory {{{2
// --------------

allc_directory_t allc_directory_new(allc_allocator_t allocator, String path)
{
    allc_directory_t self = allocator->alloc(sizeof(struct allc_directory_s));
    *self = (struct allc_directory_s){
        .allocator = allocator,
        .path = allc_strbuf_new_from_cstr(allocator, path),
    };
    return self;
}

allc_directory_t allc_directory_new_opened(allc_allocator_t allocator, String path)
{
    allc_directory_t self = allc_directory_new(allocator, path);
    allc_directory_open(&self);
    return self;
}

void allc_directory_delete(allc_directory_t self)
{
    if (self->open) {
        allc_directory_close(&self);
    }
    allc_path_delete(self->path);
    self->allocator->free(self);
}

// Directory - Object Accessors {{{3

allc_directory_status_e allc_directory_get_status(allc_directory_t *self)
{
    return (*self)->status;
}

// Directory - Object Modifiers {{{3

allc_directory_status_e allc_directory_open(allc_directory_t *self)
{
    allc_directory_t self_ = *self;
    allc_directory_status_e result = ALLC_DIRECTORY_SUCCESS;

    if (self_->status || self_->open) {
        returnd(ALLC_DIRECTORY_ERROR_BAD_STATE);
    }

    self_->handle = opendir(allc_path_to_strbuf(self_->path)->buf);
    if (self_->handle == NULL) {
        returnd(ALLC_DIRECTORY_ERROR_OPEN_FAILED);
    }

    self_->open = true;

defer: 
    self_->status = result;
    return self_->status;
}

allc_directory_status_e allc_directory_close(allc_directory_t *self)
{
    allc_directory_t self_ = *self;
    allc_directory_status_e result = ALLC_DIRECTORY_SUCCESS;

    if (self_->status || !self_->open) {
        returnd(ALLC_DIRECTORY_ERROR_BAD_STATE);
    }

    if (closedir(self_->handle) == -1) {
        returnd(ALLC_DIRECTORY_ERROR_CLOSE_FAILED);
    }

    self_->open = false;
    self_->handle = NULL;

defer:
    self_->status = result;
    return self_->status;
}

allc_directory_status_e allc_directory_read(allc_directory_t *self)
{
    allc_directory_t self_ = *self;
    allc_directory_status_e result = ALLC_DIRECTORY_SUCCESS;

    if (self_->status || !self_->open) {
        returnd(ALLC_DIRECTORY_ERROR_BAD_STATE);
    }

    errno = 0;
    self_->entry = readdir(self_->handle);
    if (self_->entry == NULL && errno != 0) {  // errno is set on an error
        returnd(ALLC_DIRECTORY_ERROR_READ_FAILED);
    } else if (self_->entry == NULL) {  // errno is not set on end of directory
        returnd(ALLC_DIRECTORY_END_OF_DIRECTORY);
    }

defer:
    self_->status = result;
    return self_->status;
}

allc_directory_status_e allc_directory_rewind(allc_directory_t *self)
{
    allc_directory_t self_ = *self;
    allc_directory_status_e result = ALLC_DIRECTORY_SUCCESS;

    if (self_->status || !self_->open) {
        returnd(ALLC_DIRECTORY_ERROR_BAD_STATE);
    }

    rewinddir(self_->handle);

defer:
    self_->status = result;
    return self_->status;
}

#endif // ALLC_DIRECTORY_IMPL
// }}}1

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker
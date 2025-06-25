/*
 *  File Handling.
 *
 *  Description
 *  ===========
 *  This module provides an abstraction layer for file management. It encapsulates 
 *  operations such as opening, reading, writing, and closing files using platform-specific
 *  system calls.
 *
 *  Options
 *  =======
 *  Compilation options to control the behavior of this file module.
 *
 *  Content
 *  =======
 *  Types and subroutines provided by this module for file handling.
 *
 *  File Object
 *  -----------
 *  [x] File
 *  [x] allc_file_new
 *  [x] allc_file_new_opened
 *  [x] allc_file_delete
 *
 *  File Object Accessors
 *  ---------------------
 *  [x] allc_file_get_status
 *
 *  File Object Modifiers
 *  ---------------------
 *  [x] allc_file_open
 *  [x] allc_file_close
 *  [x] allc_file_read
 *  [x] allc_file_write
 *  [x] allc_file_seek
 */

// Module Dependencies {{{1
// ========================

#define ALLC_IMPL // During development

#ifdef ALLC_IMPL
#   define ALLC_FILE_IMPL
#endif

#include "../path.h"
#include "../types.h"
#include "../allocator.h"

// Macros {{{1
// ===========

#define returnd(value)  \
    do {                \
        result = value; \
        goto defer;     \
    } while (0)


// Declarations {{{1
// =================

#ifndef ALLC_FILE__GUARD
#define ALLC_FILE__GUARD

// File {{{1
// =========

typedef enum allc_file_status_e
{
    ALLC_FILE_SUCCESS,
    ALLC_FILE_STATUS_EOF,
    ALLC_FILE_ERROR_BAD_STATE,
    ALLC_FILE_ERROR_OPEN_FAILED,
    ALLC_FILE_ERROR_CLOSE_FAILED,
    ALLC_FILE_ERROR_READ_FAILED,
    ALLC_FILE_ERROR_WRITE_FAILED,
    ALLC_FILE_ERROR_INVALID_MODE,
} allc_file_status_t;

#ifdef ALLC_FILE_IMPL
struct allc_file_s
{
    allc_allocator_t allocator;
    Path path;
    char mode[4];
    bool open;
    int fd;
    allc_file_status_t status;
};
typedef struct allc_file_s *allc_file_t;
#else
typedef void *allc_file_t;
#endif // ALLC_FILE__IMPL

allc_file_t allc_file_new(allc_allocator_t allocator, String path);

allc_file_t allc_file_new_opened(allc_allocator_t allocator, String path, String mode);

void allc_file_delete(allc_file_t self);

// File - Object Accessors {{{3

allc_file_status_t allc_file_get_status(allc_file_t *self);

// File - Object Modifiers {{{3

allc_file_status_t allc_file_open(allc_file_t *self, String mode);

allc_file_status_t allc_file_close(allc_file_t *self);

allc_file_status_t allc_file_read(allc_file_t *self, allc_u32_t n, CharArray to);

allc_file_status_t allc_file_write(allc_file_t *self, allc_u32_t n, String from);

allc_file_status_t allc_file_seek(allc_file_t *self, allc_i32_t offset, int whence);

#endif // ALLC_FILE__GUARD
// }}}1

/****************************************************************************/

// Defintions {{{1
// ===============

#ifndef ALLC_FILE_IMPL__GUARD
#define ALLC_FILE_IMPL__GUARD
#else
#undef ALLC_FILE_IMPL
#endif

#ifdef ALLC_FILE_IMPL

// File {{{2
// ---------

allc_file_t allc_file_new(allc_allocator_t allocator, String path)
{
    allc_file_t self = allocator->alloc(sizeof(struct allc_file_s));
    *self = (struct allc_file_s){
        .allocator = allocator,
        .path = allc_strbuf_new_from_cstr(allocator, path),
        .open = false,
        .fd = -1,
    };
    return self;
}

allc_file_t allc_file_new_opened(allc_allocator_t allocator, String path, String mode)
{
    allc_file_t self = allc_file_new(allocator, path);
    allc_file_open(&self, mode);
    return self;
}

void allc_file_delete(allc_file_t self)
{
    if (self->open) {
        allc_file_close(&self);
    }
    allc_path_delete(self->path);
    self->allocator->free(self);
}

// File - Object Accessors {{{2
// ----------------------------

allc_file_status_t allc_file_get_status(allc_file_t *self) 
{
    return (*self)->status;
}

// File - Object Modifiers {{{2
// ----------------------------

allc_file_status_t allc_file_open(allc_file_t *self, String mode)
{
    allc_file_status_t result = ALLC_FILE_SUCCESS;
    allc_file_t self_ = *self;
    int flags;

    if (self_->status || self_->open) {
        returnd(ALLC_FILE_ERROR_BAD_STATE);
    }

    if (allc_cstr_is_equal(mode, "r")) {
        flags = O_RDONLY;
    } else if (allc_cstr_is_equal(mode, "w")) {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    } else if (allc_cstr_is_equal(mode, "a")) {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    } else if (allc_cstr_is_equal(mode, "rb")) {
        flags = O_RDONLY;
    } else if (allc_cstr_is_equal(mode, "wb")) {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    } else if (allc_cstr_is_equal(mode, "ab")) {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    } else {
        returnd(ALLC_FILE_ERROR_INVALID_MODE);
    }

    allc_cstr_copy(mode, self_->mode);

    self_->fd = open(allc_path_to_strbuf(self_->path)->buf, flags, 644);
    if (self_->fd == -1) {
        returnd(ALLC_FILE_ERROR_OPEN_FAILED);
    } else {
        self_->open = true;
    }

defer:
    self_->status = result;
    return result;
}

allc_file_status_t allc_file_close(allc_file_t *self)
{
    allc_file_status_t result = ALLC_FILE_SUCCESS;
    allc_file_t self_ = *self;

    if (self_->status || !self_->open) {
        returnd(ALLC_FILE_ERROR_BAD_STATE);
    }

    if (close(self_->fd) == -1) {
        returnd(ALLC_FILE_ERROR_CLOSE_FAILED);
    }    

    self_->open = false;
    self_->mode[0] = '\0';
    self_->fd = -1;

defer:
    self_->status = result;
    return result;
}

allc_file_status_t allc_file_read(allc_file_t *self, allc_u32_t n, CharArray to)
{
    allc_file_status_t result = ALLC_FILE_SUCCESS;
    allc_file_t self_ = *self;
    allc_i32_t bytes_read = 0;

    if (self_->status || !self_->open) {
        returnd(ALLC_FILE_ERROR_BAD_STATE);
    }

    while (bytes_read < n) {
        bytes_read += read(self_->fd, to + bytes_read, n - bytes_read);
        if (bytes_read == -1) {
            returnd(ALLC_FILE_ERROR_READ_FAILED);
        } else if (bytes_read == 0) {
            returnd(ALLC_FILE_STATUS_EOF);
        }
    }

defer:
    self_->status = result;
    return result;
}

allc_file_status_t allc_file_write(allc_file_t *self, allc_u32_t n, String from)
{
    allc_file_status_t result = ALLC_FILE_SUCCESS;
    allc_file_t self_ = *self;
    allc_i32_t bytes_written = 0;

    if (self_->status || !self_->open) {
        returnd(ALLC_FILE_ERROR_BAD_STATE);
    }

    while (bytes_written < n) {
        bytes_written += write(self_->fd, from + bytes_written, n - bytes_written);
        if (bytes_written == -1) {
            returnd(ALLC_FILE_ERROR_WRITE_FAILED);
        }
    }

defer:
    self_->status = result;
    return result;
}

allc_file_status_t allc_file_seek(allc_file_t *self, allc_i32_t offset, int whence)
{
    allc_file_status_t result = ALLC_FILE_SUCCESS;
    allc_file_t self_ = *self;

    if (self_->status || !self_->open) {
        returnd(ALLC_FILE_ERROR_BAD_STATE);
    }

    if (lseek(self_->fd, offset, whence) == -1) {
        returnd(ALLC_FILE_ERROR_BAD_STATE);
    }

defer:
    self_->status = result;
    return result;
}

#endif // ALLC_FILE_IMPL
// }}}1

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker
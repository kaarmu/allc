/**
 * Directory Handling
 *
 * Description
 * ===========
 * This module provides an abstraction layer for directory management using 
 * platform-specific directory functions. It encapsulates operations such as 
 * opening, reading, and closing directories in a structured interface.
 *
 * Features:
 * - Cross-platform directory operations
 * - Built on allocator abstraction for memory management
 * - Error handling with detailed status codes
 * - Directory iteration support
 * - POSIX-compliant directory operations
 *
 * Usage Example:
 * ==============
 *
 * ```c
 * #define ALLC_IMPL
 * #include "os/directory.h"
 * 
 * int main() {
 *     allc_allocator_t allocator = allc_allocator_system();
 *     
 *     // Open a directory for reading
 *     allc_directory_t dir = allc_directory_new_opened(allocator, "/tmp");
 *     if (allc_directory_get_status(dir) == ALLC_DIRECTORY_SUCCESS) {
 *         // Read directory entries
 *         while (allc_directory_read(dir) == ALLC_DIRECTORY_SUCCESS) {
 *             printf("Entry: %s\n", allc_directory_get_entry_name(dir));
 *         }
 *         
 *         // Cleanup
 *         allc_directory_delete(dir);
 *     }
 *     
 *     return 0;
 * }
 * ```
 *
 * Options
 * =======
 *
 * ALLC_IMPL - Include implementation code
 * ALLC_DIRECTORY_IMPL - Include only directory implementation
 *
 * Authored by Kaj Munhoz Arfvidsson, 2023.
 * Edited with LLMs.
 */

#ifndef ALLC_DIRECTORY__GUARD
#define ALLC_DIRECTORY__GUARD

// Includes {{{1
// =============

#include "../types.h"
#include "../allocator.h"
#include "../cstr.h"

#include <dirent.h>
#include <errno.h>

// Macros {{{1
// ===========

#ifdef ALLC_IMPL
#define ALLC_DIRECTORY_IMPL
#endif

#define returnd(value)  \
    do {                \
        result = value; \
        goto defer;     \
    } while (0)

// Directory {{{1
// ==============

typedef enum allc_directory_status_e {
    ALLC_DIRECTORY_SUCCESS,
    ALLC_DIRECTORY_END_OF_DIRECTORY,
    ALLC_DIRECTORY_ERROR_NULL_POINTER,
    ALLC_DIRECTORY_ERROR_BAD_STATE,
    ALLC_DIRECTORY_ERROR_OPEN_FAILED,
    ALLC_DIRECTORY_ERROR_CLOSE_FAILED,
    ALLC_DIRECTORY_ERROR_READ_FAILED,
    ALLC_DIRECTORY_ERROR_ALLOCATION_FAILED,
} allc_directory_status_e;

typedef struct allc_directory_s {
    allc_allocator_t allocator;
    allc_string_t path;
    allc_bool_t is_open;
    allc_directory_status_e status;
    DIR *handle;
    struct dirent *entry;
} *allc_directory_t;

// Directory - Constructing and Destructing {{{2
// ----------------------------------------------

/* Create a new directory object with specified path */
allc_directory_t allc_directory_new(allc_allocator_t allocator, allc_string_t path);

/* Create and immediately open a directory */
allc_directory_t allc_directory_new_opened(allc_allocator_t allocator, allc_string_t path);

/* Delete the directory object and free resources */
void allc_directory_delete(allc_directory_t self);

// Directory - Object Accessors {{{2
// ---------------------------------

/* Get the current status of the directory object */
allc_directory_status_e allc_directory_get_status(allc_directory_t self);

/* Get the directory path */
allc_string_t allc_directory_get_path(allc_directory_t self);

/* Check if directory is currently open */
allc_bool_t allc_directory_is_open(allc_directory_t self);

/* Get current entry name (after successful read) */
allc_string_t allc_directory_get_entry_name(allc_directory_t self);

/* Get current entry type (after successful read) */
unsigned char allc_directory_get_entry_type(allc_directory_t self);

// Directory - Object Modifiers {{{2
// ---------------------------------

/* Open the directory for reading */
allc_directory_status_e allc_directory_open(allc_directory_t self);

/* Close the directory */
allc_directory_status_e allc_directory_close(allc_directory_t self);

/* Read next directory entry */
allc_directory_status_e allc_directory_read(allc_directory_t self);

/* Rewind directory to beginning */
allc_directory_status_e allc_directory_rewind(allc_directory_t self);

// Utility Functions {{{2
// ----------------------

/* Convert status enum to string */
allc_string_t allc_directory_status_string(allc_directory_status_e status);

#endif // ALLC_DIRECTORY__GUARD

/****************************************************************************/

// Implementation {{{1
// ===================

#ifndef ALLC_DIRECTORY_IMPL__GUARD
#define ALLC_DIRECTORY_IMPL__GUARD
#else
#undef ALLC_DIRECTORY_IMPL
#endif

#ifdef ALLC_DIRECTORY_IMPL

#include <string.h>

// Directory - Constructing and Destructing {{{2
// ----------------------------------------------

allc_directory_t allc_directory_new(allc_allocator_t allocator, allc_string_t path) {
    if (!allocator || !path) return NULL;
    
    allc_directory_t self = allc_allocator_alloc(allocator, sizeof(struct allc_directory_s));
    if (!self) return NULL;
    
    // Allocate memory for path copy
    allc_size_t path_len = allc_cstr_length(path);
    char *path_copy = allc_allocator_alloc(allocator, path_len + 1);
    if (!path_copy) {
        allc_allocator_free(allocator, self);
        return NULL;
    }
    allc_cstr_copy(path, path_copy);
    
    *self = (struct allc_directory_s){
        .allocator = allocator,
        .path = path_copy,
        .is_open = false,
        .status = ALLC_DIRECTORY_SUCCESS,
        .handle = NULL,
        .entry = NULL
    };
    
    return self;
}

allc_directory_t allc_directory_new_opened(allc_allocator_t allocator, allc_string_t path) {
    allc_directory_t self = allc_directory_new(allocator, path);
    if (!self) return NULL;
    
    allc_directory_status_e status = allc_directory_open(self);
    if (status != ALLC_DIRECTORY_SUCCESS) {
        allc_directory_delete(self);
        return NULL;
    }
    
    return self;
}

void allc_directory_delete(allc_directory_t self) {
    if (!self) return;
    
    if (self->is_open) {
        allc_directory_close(self);
    }
    
    if (self->path) {
        allc_allocator_free(self->allocator, (void*)self->path);
    }
    allc_allocator_free(self->allocator, self);
}

// Directory - Object Accessors {{{2
// ---------------------------------

allc_directory_status_e allc_directory_get_status(allc_directory_t self) {
    return self ? self->status : ALLC_DIRECTORY_ERROR_NULL_POINTER;
}

allc_string_t allc_directory_get_path(allc_directory_t self) {
    return self ? self->path : NULL;
}

allc_bool_t allc_directory_is_open(allc_directory_t self) {
    return self ? self->is_open : false;
}

allc_string_t allc_directory_get_entry_name(allc_directory_t self) {
    return (self && self->entry) ? self->entry->d_name : NULL;
}

unsigned char allc_directory_get_entry_type(allc_directory_t self) {
    return (self && self->entry) ? self->entry->d_type : 0;
}

// Directory - Object Modifiers {{{2
// ---------------------------------

allc_directory_status_e allc_directory_open(allc_directory_t self) {
    if (!self) return ALLC_DIRECTORY_ERROR_NULL_POINTER;
    if (self->is_open) return ALLC_DIRECTORY_ERROR_BAD_STATE;
    
    allc_directory_status_e result = ALLC_DIRECTORY_SUCCESS;
    
    self->handle = opendir(self->path);
    if (self->handle == NULL) {
        returnd(ALLC_DIRECTORY_ERROR_OPEN_FAILED);
    }
    
    self->is_open = true;
    self->entry = NULL;

defer:
    self->status = result;
    return result;
}

allc_directory_status_e allc_directory_close(allc_directory_t self) {
    if (!self) return ALLC_DIRECTORY_ERROR_NULL_POINTER;
    if (!self->is_open) return ALLC_DIRECTORY_ERROR_BAD_STATE;
    
    allc_directory_status_e result = ALLC_DIRECTORY_SUCCESS;
    
    if (closedir(self->handle) == -1) {
        returnd(ALLC_DIRECTORY_ERROR_CLOSE_FAILED);
    }
    
    self->is_open = false;
    self->handle = NULL;
    self->entry = NULL;

defer:
    self->status = result;
    return result;
}

allc_directory_status_e allc_directory_read(allc_directory_t self) {
    if (!self) return ALLC_DIRECTORY_ERROR_NULL_POINTER;
    if (!self->is_open) return ALLC_DIRECTORY_ERROR_BAD_STATE;
    
    allc_directory_status_e result = ALLC_DIRECTORY_SUCCESS;
    
    errno = 0;
    self->entry = readdir(self->handle);
    if (self->entry == NULL && errno != 0) {
        returnd(ALLC_DIRECTORY_ERROR_READ_FAILED);
    } else if (self->entry == NULL) {
        returnd(ALLC_DIRECTORY_END_OF_DIRECTORY);
    }

defer:
    self->status = result;
    return result;
}

allc_directory_status_e allc_directory_rewind(allc_directory_t self) {
    if (!self) return ALLC_DIRECTORY_ERROR_NULL_POINTER;
    if (!self->is_open) return ALLC_DIRECTORY_ERROR_BAD_STATE;
    
    rewinddir(self->handle);
    self->entry = NULL;
    
    self->status = ALLC_DIRECTORY_SUCCESS;
    return ALLC_DIRECTORY_SUCCESS;
}

// Utility Functions {{{2
// ----------------------

allc_string_t allc_directory_status_string(allc_directory_status_e status) {
    switch (status) {
        case ALLC_DIRECTORY_SUCCESS: return "success";
        case ALLC_DIRECTORY_END_OF_DIRECTORY: return "end of directory";
        case ALLC_DIRECTORY_ERROR_NULL_POINTER: return "null pointer";
        case ALLC_DIRECTORY_ERROR_BAD_STATE: return "bad state";
        case ALLC_DIRECTORY_ERROR_OPEN_FAILED: return "open failed";
        case ALLC_DIRECTORY_ERROR_CLOSE_FAILED: return "close failed";
        case ALLC_DIRECTORY_ERROR_READ_FAILED: return "read failed";
        case ALLC_DIRECTORY_ERROR_ALLOCATION_FAILED: return "allocation failed";
        default: return "unknown error";
    }
}

#endif // ALLC_DIRECTORY_IMPL

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker
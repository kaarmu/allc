/**
 * File Handling
 *
 * Description
 * ===========
 * This module provides an abstraction layer for file management using 
 * platform-specific file functions. It encapsulates operations such as 
 * opening, reading, writing, and closing files in a structured interface.
 *
 * Features:
 * - Cross-platform file operations
 * - Built on allocator abstraction for memory management
 * - Error handling with detailed status codes
 * - Support for text and binary modes
 * - POSIX-compliant file operations
 *
 * Usage Example:
 * ==============
 *
 * ```c
 * #define ALLC_IMPL
 * #include "os/file.h"
 * 
 * int main() {
 *     allc_allocator_t allocator = allc_allocator_system();
 *     
 *     // Create and open a file for writing
 *     allc_file_t file = allc_file_new_opened(allocator, "test.txt", "w");
 *     if (allc_file_get_status(&file) == ALLC_FILE_SUCCESS) {
 *         // Write data to file
 *         char data[] = "Hello, World!";
 *         allc_file_write(&file, strlen(data), data);
 *         
 *         // Close and cleanup
 *         allc_file_delete(file);
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
 * ALLC_FILE_IMPL - Include only file implementation
 *
 * Authored by Kaj Munhoz Arfvidsson, 2023.
 * Edited with LLMs.
 */

#ifndef ALLC_FILE__GUARD
#define ALLC_FILE__GUARD

// Includes {{{1
// =============

#include "../types.h"
#include "../allocator.h"
#include "../cstr.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

// Macros {{{1
// ===========

#ifdef ALLC_IMPL
#define ALLC_FILE_IMPL
#endif

#define returnd(value)  \
    do {                \
        result = value; \
        goto defer;     \
    } while (0)

// File {{{1
// =========

typedef enum allc_file_status_e {
    ALLC_FILE_SUCCESS,
    ALLC_FILE_STATUS_EOF,
    ALLC_FILE_ERROR_NULL_POINTER,
    ALLC_FILE_ERROR_BAD_STATE,
    ALLC_FILE_ERROR_OPEN_FAILED,
    ALLC_FILE_ERROR_CLOSE_FAILED,
    ALLC_FILE_ERROR_READ_FAILED,
    ALLC_FILE_ERROR_WRITE_FAILED,
    ALLC_FILE_ERROR_SEEK_FAILED,
    ALLC_FILE_ERROR_INVALID_MODE,
    ALLC_FILE_ERROR_ALLOCATION_FAILED,
} allc_file_status_e;

typedef struct allc_file_s {
    allc_allocator_t allocator;
    allc_string_t path;
    char mode[8];
    allc_bool_t is_open;
    int fd;
    allc_file_status_e status;
} *allc_file_t;

// File - Constructing and Destructing {{{2
// ----------------------------------------

/* Create a new file object with specified path */
allc_file_t allc_file_new(allc_allocator_t allocator, allc_string_t path);

/* Create and immediately open a file with specified mode */
allc_file_t allc_file_new_opened(allc_allocator_t allocator, allc_string_t path, allc_string_t mode);

/* Delete the file object and free resources */
void allc_file_delete(allc_file_t self);

// File - Object Accessors {{{2
// ----------------------------

/* Get the current status of the file object */
allc_file_status_e allc_file_get_status(allc_file_t self);

/* Get the file path */
allc_string_t allc_file_get_path(allc_file_t self);

/* Check if file is currently open */
allc_bool_t allc_file_is_open(allc_file_t self);

/* Get current file mode */
allc_string_t allc_file_get_mode(allc_file_t self);

// File - Object Modifiers {{{2
// ----------------------------

/* Open the file with specified mode */
allc_file_status_e allc_file_open(allc_file_t self, allc_string_t mode);

/* Close the file */
allc_file_status_e allc_file_close(allc_file_t self);

/* Read n bytes from file into buffer */
allc_file_status_e allc_file_read(allc_file_t self, allc_size_t n, char *buffer, allc_size_t *bytes_read);

/* Write n bytes from buffer to file */
allc_file_status_e allc_file_write(allc_file_t self, allc_size_t n, const char *buffer, allc_size_t *bytes_written);

/* Seek to position in file */
allc_file_status_e allc_file_seek(allc_file_t self, off_t offset, int whence);

/* Get current file position */
allc_file_status_e allc_file_tell(allc_file_t self, off_t *position);

/* Get file size */
allc_file_status_e allc_file_size(allc_file_t self, off_t *size);

// Utility Functions {{{2
// ----------------------

/* Convert status enum to string */
allc_string_t allc_file_status_string(allc_file_status_e status);

#endif // ALLC_FILE__GUARD

/****************************************************************************/

// Implementation {{{1
// ===================

#ifndef ALLC_FILE_IMPL__GUARD
#define ALLC_FILE_IMPL__GUARD
#else
#undef ALLC_FILE_IMPL
#endif

#ifdef ALLC_FILE_IMPL

#include <string.h>
#include <errno.h>
#include <sys/stat.h>

// File - Constructing and Destructing {{{2
// ----------------------------------------

allc_file_t allc_file_new(allc_allocator_t allocator, allc_string_t path) {
    if (!allocator || !path) return NULL;
    
    allc_file_t self = allc_allocator_alloc(allocator, sizeof(struct allc_file_s));
    if (!self) return NULL;
    
    // Allocate memory for path copy
    allc_size_t path_len = allc_cstr_length(path);
    char *path_copy = allc_allocator_alloc(allocator, path_len + 1);
    if (!path_copy) {
        allc_allocator_free(allocator, self);
        return NULL;
    }
    allc_cstr_copy(path, path_copy);
    
    *self = (struct allc_file_s){
        .allocator = allocator,
        .path = path_copy,
        .mode = {0},
        .is_open = false,
        .fd = -1,
        .status = ALLC_FILE_SUCCESS
    };
    
    return self;
}

allc_file_t allc_file_new_opened(allc_allocator_t allocator, allc_string_t path, allc_string_t mode) {
    allc_file_t self = allc_file_new(allocator, path);
    if (!self) return NULL;
    
    allc_file_status_e status = allc_file_open(self, mode);
    if (status != ALLC_FILE_SUCCESS) {
        allc_file_delete(self);
        return NULL;
    }
    
    return self;
}

void allc_file_delete(allc_file_t self) {
    if (!self) return;
    
    if (self->is_open) {
        allc_file_close(self);
    }
    
    if (self->path) {
        allc_allocator_free(self->allocator, (void*)self->path);
    }
    allc_allocator_free(self->allocator, self);
}

// File - Object Accessors {{{2
// ----------------------------

allc_file_status_e allc_file_get_status(allc_file_t self) {
    return self ? self->status : ALLC_FILE_ERROR_NULL_POINTER;
}

allc_string_t allc_file_get_path(allc_file_t self) {
    return self ? self->path : NULL;
}

allc_bool_t allc_file_is_open(allc_file_t self) {
    return self ? self->is_open : false;
}

allc_string_t allc_file_get_mode(allc_file_t self) {
    return self ? self->mode : NULL;
}

// File - Object Modifiers {{{2
// ----------------------------

allc_file_status_e allc_file_open(allc_file_t self, allc_string_t mode) {
    if (!self) return ALLC_FILE_ERROR_NULL_POINTER;
    if (!mode) return ALLC_FILE_ERROR_INVALID_MODE;
    if (self->is_open) return ALLC_FILE_ERROR_BAD_STATE;
    
    allc_file_status_e result = ALLC_FILE_SUCCESS;
    int flags = 0;
    mode_t file_mode = 0644;
    
    // Parse mode string
    if (allc_cstr_is_equal(mode, "r")) {
        flags = O_RDONLY;
    } else if (allc_cstr_is_equal(mode, "w")) {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    } else if (allc_cstr_is_equal(mode, "a")) {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    } else if (allc_cstr_is_equal(mode, "r+")) {
        flags = O_RDWR;
    } else if (allc_cstr_is_equal(mode, "w+")) {
        flags = O_RDWR | O_CREAT | O_TRUNC;
    } else if (allc_cstr_is_equal(mode, "a+")) {
        flags = O_RDWR | O_CREAT | O_APPEND;
    } else if (allc_cstr_is_equal(mode, "rb")) {
        flags = O_RDONLY;
    } else if (allc_cstr_is_equal(mode, "wb")) {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    } else if (allc_cstr_is_equal(mode, "ab")) {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    } else if (allc_cstr_is_equal(mode, "rb+") || allc_cstr_is_equal(mode, "r+b")) {
        flags = O_RDWR;
    } else if (allc_cstr_is_equal(mode, "wb+") || allc_cstr_is_equal(mode, "w+b")) {
        flags = O_RDWR | O_CREAT | O_TRUNC;
    } else if (allc_cstr_is_equal(mode, "ab+") || allc_cstr_is_equal(mode, "a+b")) {
        flags = O_RDWR | O_CREAT | O_APPEND;
    } else {
        returnd(ALLC_FILE_ERROR_INVALID_MODE);
    }
    
    // Copy mode string
    allc_cstr_copy(mode, self->mode);
    
    // Open the file
    self->fd = open(self->path, flags, file_mode);
    if (self->fd == -1) {
        returnd(ALLC_FILE_ERROR_OPEN_FAILED);
    }
    
    self->is_open = true;

defer:
    self->status = result;
    return result;
}

allc_file_status_e allc_file_close(allc_file_t self) {
    if (!self) return ALLC_FILE_ERROR_NULL_POINTER;
    if (!self->is_open) return ALLC_FILE_ERROR_BAD_STATE;
    
    allc_file_status_e result = ALLC_FILE_SUCCESS;
    
    if (close(self->fd) == -1) {
        returnd(ALLC_FILE_ERROR_CLOSE_FAILED);
    }
    
    self->is_open = false;
    self->fd = -1;
    self->mode[0] = '\0';

defer:
    self->status = result;
    return result;
}

allc_file_status_e allc_file_read(allc_file_t self, allc_size_t n, char *buffer, allc_size_t *bytes_read) {
    if (!self) return ALLC_FILE_ERROR_NULL_POINTER;
    if (!buffer) return ALLC_FILE_ERROR_NULL_POINTER;
    if (!self->is_open) return ALLC_FILE_ERROR_BAD_STATE;
    
    allc_file_status_e result = ALLC_FILE_SUCCESS;
    ssize_t read_result = read(self->fd, buffer, n);
    
    if (read_result == -1) {
        if (bytes_read) *bytes_read = 0;
        returnd(ALLC_FILE_ERROR_READ_FAILED);
    } else if (read_result == 0) {
        if (bytes_read) *bytes_read = 0;
        returnd(ALLC_FILE_STATUS_EOF);
    }
    
    if (bytes_read) *bytes_read = (allc_size_t)read_result;

defer:
    self->status = result;
    return result;
}

allc_file_status_e allc_file_write(allc_file_t self, allc_size_t n, const char *buffer, allc_size_t *bytes_written) {
    if (!self) return ALLC_FILE_ERROR_NULL_POINTER;
    if (!buffer) return ALLC_FILE_ERROR_NULL_POINTER;
    if (!self->is_open) return ALLC_FILE_ERROR_BAD_STATE;
    
    allc_file_status_e result = ALLC_FILE_SUCCESS;
    ssize_t write_result = write(self->fd, buffer, n);
    
    if (write_result == -1) {
        if (bytes_written) *bytes_written = 0;
        returnd(ALLC_FILE_ERROR_WRITE_FAILED);
    }
    
    if (bytes_written) *bytes_written = (allc_size_t)write_result;

defer:
    self->status = result;
    return result;
}

allc_file_status_e allc_file_seek(allc_file_t self, off_t offset, int whence) {
    if (!self) return ALLC_FILE_ERROR_NULL_POINTER;
    if (!self->is_open) return ALLC_FILE_ERROR_BAD_STATE;
    
    allc_file_status_e result = ALLC_FILE_SUCCESS;
    
    if (lseek(self->fd, offset, whence) == -1) {
        returnd(ALLC_FILE_ERROR_SEEK_FAILED);
    }

defer:
    self->status = result;
    return result;
}

allc_file_status_e allc_file_tell(allc_file_t self, off_t *position) {
    if (!self) return ALLC_FILE_ERROR_NULL_POINTER;
    if (!position) return ALLC_FILE_ERROR_NULL_POINTER;
    if (!self->is_open) return ALLC_FILE_ERROR_BAD_STATE;
    
    allc_file_status_e result = ALLC_FILE_SUCCESS;
    off_t pos = lseek(self->fd, 0, SEEK_CUR);
    
    if (pos == -1) {
        *position = 0;
        returnd(ALLC_FILE_ERROR_SEEK_FAILED);
    }
    
    *position = pos;

defer:
    self->status = result;
    return result;
}

allc_file_status_e allc_file_size(allc_file_t self, off_t *size) {
    if (!self) return ALLC_FILE_ERROR_NULL_POINTER;
    if (!size) return ALLC_FILE_ERROR_NULL_POINTER;
    if (!self->is_open) return ALLC_FILE_ERROR_BAD_STATE;
    
    allc_file_status_e result = ALLC_FILE_SUCCESS;
    struct stat st;
    
    if (fstat(self->fd, &st) == -1) {
        *size = 0;
        returnd(ALLC_FILE_ERROR_SEEK_FAILED);
    }
    
    *size = st.st_size;

defer:
    self->status = result;
    return result;
}

// Utility Functions {{{2
// ----------------------

allc_string_t allc_file_status_string(allc_file_status_e status) {
    switch (status) {
        case ALLC_FILE_SUCCESS: return "success";
        case ALLC_FILE_STATUS_EOF: return "end of file";
        case ALLC_FILE_ERROR_NULL_POINTER: return "null pointer";
        case ALLC_FILE_ERROR_BAD_STATE: return "bad state";
        case ALLC_FILE_ERROR_OPEN_FAILED: return "open failed";
        case ALLC_FILE_ERROR_CLOSE_FAILED: return "close failed";
        case ALLC_FILE_ERROR_READ_FAILED: return "read failed";
        case ALLC_FILE_ERROR_WRITE_FAILED: return "write failed";
        case ALLC_FILE_ERROR_SEEK_FAILED: return "seek failed";
        case ALLC_FILE_ERROR_INVALID_MODE: return "invalid mode";
        case ALLC_FILE_ERROR_ALLOCATION_FAILED: return "allocation failed";
        default: return "unknown error";
    }
}

#endif // ALLC_FILE_IMPL

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker
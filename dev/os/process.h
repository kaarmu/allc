/**
 * Process Management
 *
 * Description
 * ===========
 *
 * Cross-platform process creation, management, and control. This module provides
 * an abstraction layer for process operations including spawning, waiting,
 * communication, and termination.
 *
 * Features:
 * - Process creation with custom arguments and environment
 * - Process waiting and status retrieval
 * - Process termination and signal handling
 * - Process information queries (PID, exit status, etc.)
 * - Cross-platform compatibility abstractions
 * - Process output redirection and pipes
 *
 * Usage Example:
 * ==============
 *
 * ```c
 * #define ALLC_IMPL
 * #include "os/process.h"
 * 
 * int main() {
 *     allc_allocator_t allocator = allc_allocator_system();
 *     
 *     // Create and spawn a process
 *     allc_process_t proc = allc_process_new(allocator);
 *     char *args[] = {"/bin/ls", "-la", NULL};
 *     
 *     if (allc_process_spawn(&proc, "/bin/ls", args, NULL) == ALLC_PROCESS_SUCCESS) {
 *         // Wait for completion
 *         allc_i32_t exit_code = allc_process_wait(&proc);
 *         printf("Process exited with code: %d\n", exit_code);
 *     }
 *     
 *     allc_process_delete(&proc);
 *     return 0;
 * }
 * ```
 *
 * Options
 * =======
 *
 * ALLC_IMPL - Include implementation code
 * ALLC_PROCESS_IMPL - Include only process implementation
 *
 * Authored by Kaj Munhoz Arfvidsson, 2023.
 * Edited with LLMs.
 */

#ifndef ALLC_PROCESS__GUARD
#define ALLC_PROCESS__GUARD

// Module Dependencies {{{1
// ========================

#ifdef ALLC_IMPL
#define ALLC_TYPES_IMPL
#define ALLC_ALLOCATOR_IMPL
#define ALLC_PROCESS_IMPL
#endif

#include "../types.h"
#include "../allocator.h"

#include <sys/types.h>

// Type Definitions {{{1
// =====================

// Process status codes
typedef enum {
    ALLC_PROCESS_SUCCESS = 0,
    ALLC_PROCESS_ERROR_INVALID_ARGS = 1,
    ALLC_PROCESS_ERROR_FORK_FAILED = 2,
    ALLC_PROCESS_ERROR_EXEC_FAILED = 3,
    ALLC_PROCESS_ERROR_WAIT_FAILED = 4,
    ALLC_PROCESS_ERROR_NOT_RUNNING = 5,
    ALLC_PROCESS_ERROR_KILL_FAILED = 6,
    ALLC_PROCESS_ERROR_PIPE_FAILED = 7,
    ALLC_PROCESS_ERROR_NULL_POINTER = 8,
    ALLC_PROCESS_ERROR_WRITE_FAILED = 9
} allc_process_status_e;

// Process state
typedef enum {
    ALLC_PROCESS_STATE_NOT_STARTED = 0,
    ALLC_PROCESS_STATE_RUNNING = 1,
    ALLC_PROCESS_STATE_FINISHED = 2,
    ALLC_PROCESS_STATE_TERMINATED = 3,
    ALLC_PROCESS_STATE_ERROR = 4
} allc_process_state_e;

// Process spawn options
typedef struct {
    allc_bool_t capture_stdout;
    allc_bool_t capture_stderr;
    allc_bool_t inherit_environment;
    allc_string_t working_directory;
    allc_u32_t timeout_seconds;
} allc_process_options_t;

// Forward declaration for opaque type
typedef struct allc_process_s allc_process_s;
typedef allc_process_s *allc_process_t;

#ifdef ALLC_PROCESS_IMPL
// Internal process structure definition
struct allc_process_s {
    allc_allocator_t allocator;
    pid_t pid;
    allc_process_state_e state;
    allc_i32_t exit_code;
    allc_process_options_t options;
    allc_string_t command;
    char **argv;
    char **envp;
    int stdin_fd;
    int stdout_fd;
    int stderr_fd;
};
#endif

// Function Declarations {{{1
// ==========================

/* Process Creation and Management */

/**
 * Create a new process object.
 */
allc_process_t allc_process_new(allc_allocator_t allocator);

/**
 * Delete a process object and free resources.
 */
void allc_process_delete(allc_process_t *self);

/**
 * Get default process options.
 */
allc_process_options_t allc_process_default_options(void);

/* Process Execution */

/**
 * Spawn a process with the given command and arguments.
 */
allc_process_status_e allc_process_spawn(allc_process_t *self, 
                                       allc_string_t command,
                                       char *const argv[],
                                       char *const envp[]);

/**
 * Spawn a process with options.
 */
allc_process_status_e allc_process_spawn_with_options(allc_process_t *self,
                                                    allc_string_t command,
                                                    char *const argv[],
                                                    char *const envp[],
                                                    const allc_process_options_t *options);

/**
 * Spawn a process and wait for completion (convenience function).
 */
allc_i32_t allc_process_spawn_wait(allc_string_t command, char *const argv[]);

/**
 * Execute a simple command with arguments (convenience function).
 */
allc_i32_t allc_process_execute(allc_string_t command, char *const argv[]);

/* Process Control */

/**
 * Wait for the process to complete.
 */
allc_i32_t allc_process_wait(allc_process_t *self);

/**
 * Wait for the process with a timeout.
 */
allc_process_status_e allc_process_wait_timeout(allc_process_t *self, 
                                              allc_u32_t timeout_seconds,
                                              allc_i32_t *exit_code);

/**
 * Check if the process is still running (non-blocking).
 */
allc_bool_t allc_process_is_running(allc_process_t *self);

/**
 * Terminate the process gracefully (SIGTERM).
 */
allc_process_status_e allc_process_terminate(allc_process_t *self);

/**
 * Kill the process forcefully (SIGKILL).
 */
allc_process_status_e allc_process_kill(allc_process_t *self);

/**
 * Send a signal to the process.
 */
allc_process_status_e allc_process_signal(allc_process_t *self, allc_i32_t signal);

/* Process Information */

/**
 * Get the process ID.
 */
pid_t allc_process_get_pid(allc_process_t self);

/**
 * Get the process state.
 */
allc_process_state_e allc_process_get_state(allc_process_t self);

/**
 * Get the exit code (only valid after process has finished).
 */
allc_i32_t allc_process_get_exit_code(allc_process_t self);

/**
 * Get the command used to start the process.
 */
allc_string_t allc_process_get_command(allc_process_t self);

/* Process I/O */

/**
 * Write data to the process stdin.
 */
allc_process_status_e allc_process_write_stdin(allc_process_t *self,
                                             const void *data,
                                             allc_size_t size);

/**
 * Read data from the process stdout.
 */
allc_size_t allc_process_read_stdout(allc_process_t *self,
                                   void *buffer,
                                   allc_size_t buffer_size);

/**
 * Read data from the process stderr.
 */
allc_size_t allc_process_read_stderr(allc_process_t *self,
                                   void *buffer,
                                   allc_size_t buffer_size);

/* Utility Functions */

/**
 * Get the current process ID.
 */
pid_t allc_process_current_pid(void);

/**
 * Get the parent process ID.
 */
pid_t allc_process_parent_pid(void);

/**
 * Exit the current process with the given status.
 */
void allc_process_exit(allc_i32_t status);

/**
 * Exit the current process without cleanup.
 */
void allc_process_exit_fast(allc_i32_t status);

/**
 * Check if a process with the given PID exists.
 */
allc_bool_t allc_process_exists(pid_t pid);

/**
 * Get string representation of process status.
 */
allc_string_t allc_process_status_string(allc_process_status_e status);

/**
 * Get string representation of process state.
 */
allc_string_t allc_process_state_string(allc_process_state_e state);

#endif // ALLC_PROCESS__GUARD

/****************************************************************************/

#ifndef ALLC_PROCESS_IMPL__GUARD
#define ALLC_PROCESS_IMPL__GUARD
#else
#undef ALLC_PROCESS_IMPL
#endif

#ifdef ALLC_PROCESS_IMPL

// Includes {{{1
// =============

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

// Ensure kill is declared
extern int kill(pid_t pid, int sig);

// Internal Helper Functions {{{1
// ==============================

/**
 * Helper macro for error handling with cleanup.
 */
#define return_(value)  \
    do {                \
        result = value; \
        goto defer;     \
    } while (0)

/**
 * Helper function to duplicate string array.
 */
static char **allc_process_dup_string_array(allc_allocator_t allocator, char *const arr[]) {
    if (!arr) return NULL;
    
    // Count elements
    allc_size_t count = 0;
    while (arr[count]) count++;
    
    // Allocate array of pointers
    char **result = allc_allocator_alloc(allocator, (count + 1) * sizeof(char*));
    if (!result) return NULL;
    
    // Copy strings
    for (allc_size_t i = 0; i < count; i++) {
        allc_size_t len = strlen(arr[i]);
        result[i] = allc_allocator_alloc(allocator, len + 1);
        if (!result[i]) {
            // Cleanup on failure
            for (allc_size_t j = 0; j < i; j++) {
                allc_allocator_free(allocator, result[j]);
            }
            allc_allocator_free(allocator, result);
            return NULL;
        }
        strcpy(result[i], arr[i]);
    }
    result[count] = NULL;
    
    return result;
}

/**
 * Helper function to free string array.
 */
static void allc_process_free_string_array(allc_allocator_t allocator, char **arr) {
    if (!arr) return;
    
    for (allc_size_t i = 0; arr[i]; i++) {
        allc_allocator_free(allocator, arr[i]);
    }
    allc_allocator_free(allocator, arr);
}

/**
 * Update process state by checking if it's still running.
 */
static void allc_process_update_state(allc_process_t self) {
    if (!self || self->state != ALLC_PROCESS_STATE_RUNNING) return;
    
    int status;
    pid_t result = waitpid(self->pid, &status, WNOHANG);
    
    if (result == 0) {
        // Process is still running
        return;
    } else if (result == self->pid) {
        // Process has finished
        if (WIFEXITED(status)) {
            self->exit_code = WEXITSTATUS(status);
            self->state = ALLC_PROCESS_STATE_FINISHED;
        } else if (WIFSIGNALED(status)) {
            self->exit_code = -WTERMSIG(status);
            self->state = ALLC_PROCESS_STATE_TERMINATED;
        }
    } else {
        // Error occurred
        self->state = ALLC_PROCESS_STATE_ERROR;
    }
}

// Process Creation and Management {{{1
// ====================================

allc_process_t allc_process_new(allc_allocator_t allocator) {
    allc_process_t self = allc_allocator_alloc(allocator, sizeof(struct allc_process_s));
    if (!self) return NULL;
    
    memset(self, 0, sizeof(struct allc_process_s));
    self->allocator = allocator;
    self->pid = -1;
    self->state = ALLC_PROCESS_STATE_NOT_STARTED;
    self->exit_code = 0;
    self->stdin_fd = -1;
    self->stdout_fd = -1;
    self->stderr_fd = -1;
    
    return self;
}

void allc_process_delete(allc_process_t *self) {
    if (!self || !*self) return;
    
    allc_process_t proc = *self;
    
    // Close file descriptors
    if (proc->stdin_fd >= 0) close(proc->stdin_fd);
    if (proc->stdout_fd >= 0) close(proc->stdout_fd);
    if (proc->stderr_fd >= 0) close(proc->stderr_fd);
    
    // Free string arrays
    allc_process_free_string_array(proc->allocator, proc->argv);
    allc_process_free_string_array(proc->allocator, proc->envp);
    
    // Free command string if allocated
    if (proc->command) {
        allc_allocator_free(proc->allocator, (void*)proc->command);
    }
    
    allc_allocator_free(proc->allocator, proc);
    *self = NULL;
}

allc_process_options_t allc_process_default_options(void) {
    allc_process_options_t options = {0};
    options.capture_stdout = false;
    options.capture_stderr = false;
    options.inherit_environment = true;
    options.working_directory = NULL;
    options.timeout_seconds = 0; // No timeout
    return options;
}

// Process Execution {{{1
// ======================

allc_process_status_e allc_process_spawn(allc_process_t *self, 
                                       allc_string_t command,
                                       char *const argv[],
                                       char *const envp[]) {
    allc_process_options_t options = allc_process_default_options();
    return allc_process_spawn_with_options(self, command, argv, envp, &options);
}

allc_process_status_e allc_process_spawn_with_options(allc_process_t *self,
                                                    allc_string_t command,
                                                    char *const argv[],
                                                    char *const envp[],
                                                    const allc_process_options_t *options) {
    if (!self || !*self || !command) return ALLC_PROCESS_ERROR_NULL_POINTER;
    
    allc_process_t proc = *self;
    if (proc->state != ALLC_PROCESS_STATE_NOT_STARTED) return ALLC_PROCESS_ERROR_INVALID_ARGS;
    
    allc_process_status_e result = ALLC_PROCESS_SUCCESS;
    // Note: pipes_stdin would be used for process input redirection
    // Currently not implemented, so commented out to avoid unused variable warning
    // int pipes_stdin[2] = {-1, -1};
    int pipes_stdout[2] = {-1, -1};
    int pipes_stderr[2] = {-1, -1};
    
    // Store options
    proc->options = options ? *options : allc_process_default_options();
    
    // Store command
    allc_size_t cmd_len = strlen(command);
    proc->command = allc_allocator_alloc(proc->allocator, cmd_len + 1);
    if (!proc->command) return_(ALLC_PROCESS_ERROR_FORK_FAILED);
    strcpy((char*)proc->command, command);
    
    // Copy arguments and environment
    proc->argv = allc_process_dup_string_array(proc->allocator, argv);
    if (envp) {
        proc->envp = allc_process_dup_string_array(proc->allocator, envp);
    }
    
    // Create pipes if needed
    if (proc->options.capture_stdout && pipe(pipes_stdout) == -1) {
        return_(ALLC_PROCESS_ERROR_PIPE_FAILED);
    }
    if (proc->options.capture_stderr && pipe(pipes_stderr) == -1) {
        return_(ALLC_PROCESS_ERROR_PIPE_FAILED);
    }
    
    // Fork process
    proc->pid = fork();
    if (proc->pid == -1) {
        return_(ALLC_PROCESS_ERROR_FORK_FAILED);
    }
    
    if (proc->pid == 0) {
        // Child process
        
        // Setup pipe redirection
        if (proc->options.capture_stdout) {
            close(pipes_stdout[0]); // Close read end
            dup2(pipes_stdout[1], STDOUT_FILENO);
            close(pipes_stdout[1]);
        }
        
        if (proc->options.capture_stderr) {
            close(pipes_stderr[0]); // Close read end
            dup2(pipes_stderr[1], STDERR_FILENO);
            close(pipes_stderr[1]);
        }
        
        // Change working directory if specified
        if (proc->options.working_directory) {
            if (chdir(proc->options.working_directory) == -1) {
                _exit(127);
            }
        }
        
        // Execute command
        if (proc->envp) {
            execve(command, proc->argv ? proc->argv : argv, proc->envp);
        } else if (proc->options.inherit_environment) {
            execv(command, proc->argv ? proc->argv : argv);
        } else {
            execve(command, proc->argv ? proc->argv : argv, NULL);
        }
        
        // If we get here, exec failed
        _exit(127);
    } else {
        // Parent process
        proc->state = ALLC_PROCESS_STATE_RUNNING;
        
        // Setup parent side of pipes
        if (proc->options.capture_stdout) {
            close(pipes_stdout[1]); // Close write end
            proc->stdout_fd = pipes_stdout[0];
        }
        
        if (proc->options.capture_stderr) {
            close(pipes_stderr[1]); // Close write end
            proc->stderr_fd = pipes_stderr[0];
        }
    }

defer:
    // Cleanup on error
    if (result != ALLC_PROCESS_SUCCESS) {
        if (pipes_stdout[0] >= 0) close(pipes_stdout[0]);
        if (pipes_stdout[1] >= 0) close(pipes_stdout[1]);
        if (pipes_stderr[0] >= 0) close(pipes_stderr[0]);
        if (pipes_stderr[1] >= 0) close(pipes_stderr[1]);
    }
    
    return result;
}

allc_i32_t allc_process_spawn_wait(allc_string_t command, char *const argv[]) {
    allc_allocator_t allocator = allc_allocator_system();
    allc_process_t proc = allc_process_new(allocator);
    if (!proc) return -1;
    
    allc_process_status_e status = allc_process_spawn(&proc, command, argv, NULL);
    if (status != ALLC_PROCESS_SUCCESS) {
        allc_process_delete(&proc);
        return -1;
    }
    
    allc_i32_t exit_code = allc_process_wait(&proc);
    allc_process_delete(&proc);
    
    return exit_code;
}

allc_i32_t allc_process_execute(allc_string_t command, char *const argv[]) {
    return allc_process_spawn_wait(command, argv);
}

// Process Control {{{1
// ====================

allc_i32_t allc_process_wait(allc_process_t *self) {
    if (!self || !*self || (*self)->state != ALLC_PROCESS_STATE_RUNNING) {
        return (self && *self) ? (*self)->exit_code : -1;
    }
    
    allc_process_t proc = *self;
    int status;
    pid_t result = waitpid(proc->pid, &status, 0);
    
    if (result == proc->pid) {
        if (WIFEXITED(status)) {
            proc->exit_code = WEXITSTATUS(status);
            proc->state = ALLC_PROCESS_STATE_FINISHED;
        } else if (WIFSIGNALED(status)) {
            proc->exit_code = -WTERMSIG(status);
            proc->state = ALLC_PROCESS_STATE_TERMINATED;
        }
    } else {
        proc->state = ALLC_PROCESS_STATE_ERROR;
        proc->exit_code = -1;
    }
    
    return proc->exit_code;
}

allc_process_status_e allc_process_wait_timeout(allc_process_t *self, 
                                              allc_u32_t timeout_seconds,
                                              allc_i32_t *exit_code) {
    if (!self || !*self || !exit_code) return ALLC_PROCESS_ERROR_NULL_POINTER;
    
    allc_process_t proc = *self;
    if (proc->state != ALLC_PROCESS_STATE_RUNNING) {
        *exit_code = proc->exit_code;
        return ALLC_PROCESS_SUCCESS;
    }
    
    // Simple timeout implementation using alarm (Unix-specific)
    // For a more robust implementation, consider using select() or poll()
    // TODO: Actually implement timeout functionality using timeout_seconds parameter
    (void)timeout_seconds;  // Suppress unused parameter warning
    
    int status;
    pid_t result = waitpid(proc->pid, &status, WNOHANG);
    
    if (result == 0) {
        // Process still running - would need proper timeout implementation
        return ALLC_PROCESS_ERROR_WAIT_FAILED;
    } else if (result == proc->pid) {
        if (WIFEXITED(status)) {
            proc->exit_code = WEXITSTATUS(status);
            proc->state = ALLC_PROCESS_STATE_FINISHED;
        } else if (WIFSIGNALED(status)) {
            proc->exit_code = -WTERMSIG(status);
            proc->state = ALLC_PROCESS_STATE_TERMINATED;
        }
        *exit_code = proc->exit_code;
        return ALLC_PROCESS_SUCCESS;
    } else {
        proc->state = ALLC_PROCESS_STATE_ERROR;
        return ALLC_PROCESS_ERROR_WAIT_FAILED;
    }
}

allc_bool_t allc_process_is_running(allc_process_t *self) {
    if (!self || !*self) return false;
    
    allc_process_update_state(*self);
    return (*self)->state == ALLC_PROCESS_STATE_RUNNING;
}

allc_process_status_e allc_process_terminate(allc_process_t *self) {
    return allc_process_signal(self, SIGTERM);
}

allc_process_status_e allc_process_kill(allc_process_t *self) {
    return allc_process_signal(self, SIGKILL);
}

allc_process_status_e allc_process_signal(allc_process_t *self, allc_i32_t signal) {
    if (!self || !*self || (*self)->pid <= 0) return ALLC_PROCESS_ERROR_NULL_POINTER;
    
    allc_process_t proc = *self;
    if (proc->state != ALLC_PROCESS_STATE_RUNNING) return ALLC_PROCESS_ERROR_NOT_RUNNING;
    
    if (kill(proc->pid, signal) == -1) {
        return ALLC_PROCESS_ERROR_KILL_FAILED;
    }
    
    return ALLC_PROCESS_SUCCESS;
}

// Process Information {{{1
// ========================

pid_t allc_process_get_pid(allc_process_t self) {
    return self ? self->pid : -1;
}

allc_process_state_e allc_process_get_state(allc_process_t self) {
    return self ? self->state : ALLC_PROCESS_STATE_ERROR;
}

allc_i32_t allc_process_get_exit_code(allc_process_t self) {
    return self ? self->exit_code : -1;
}

allc_string_t allc_process_get_command(allc_process_t self) {
    return self ? self->command : NULL;
}

// Process I/O {{{1
// ================

allc_process_status_e allc_process_write_stdin(allc_process_t *self,
                                             const void *data,
                                             allc_size_t size) {
    if (!self || !*self || !data || (*self)->stdin_fd < 0) return ALLC_PROCESS_ERROR_NULL_POINTER;
    
    ssize_t written = write((*self)->stdin_fd, data, size);
    return (written == (ssize_t)size) ? ALLC_PROCESS_SUCCESS : ALLC_PROCESS_ERROR_WRITE_FAILED;
}

allc_size_t allc_process_read_stdout(allc_process_t *self,
                                   void *buffer,
                                   allc_size_t buffer_size) {
    if (!self || !*self || !buffer || (*self)->stdout_fd < 0) return 0;
    
    ssize_t bytes_read = read((*self)->stdout_fd, buffer, buffer_size);
    return bytes_read > 0 ? (allc_size_t)bytes_read : 0;
}

allc_size_t allc_process_read_stderr(allc_process_t *self,
                                   void *buffer,
                                   allc_size_t buffer_size) {
    if (!self || !*self || !buffer || (*self)->stderr_fd < 0) return 0;
    
    ssize_t bytes_read = read((*self)->stderr_fd, buffer, buffer_size);
    return bytes_read > 0 ? (allc_size_t)bytes_read : 0;
}

// Utility Functions {{{1
// ======================

pid_t allc_process_current_pid(void) {
    return getpid();
}

pid_t allc_process_parent_pid(void) {
    return getppid();
}

void allc_process_exit(allc_i32_t status) {
    exit(status);
}

void allc_process_exit_fast(allc_i32_t status) {
    _exit(status);
}

allc_bool_t allc_process_exists(pid_t pid) {
    return kill(pid, 0) == 0;
}

allc_string_t allc_process_status_string(allc_process_status_e status) {
    switch (status) {
        case ALLC_PROCESS_SUCCESS: return "Success";
        case ALLC_PROCESS_ERROR_INVALID_ARGS: return "Invalid arguments";
        case ALLC_PROCESS_ERROR_FORK_FAILED: return "Fork failed";
        case ALLC_PROCESS_ERROR_EXEC_FAILED: return "Exec failed";
        case ALLC_PROCESS_ERROR_WAIT_FAILED: return "Wait failed";
        case ALLC_PROCESS_ERROR_NOT_RUNNING: return "Process not running";
        case ALLC_PROCESS_ERROR_KILL_FAILED: return "Kill failed";
        case ALLC_PROCESS_ERROR_PIPE_FAILED: return "Pipe creation failed";
        case ALLC_PROCESS_ERROR_NULL_POINTER: return "Null pointer";
        case ALLC_PROCESS_ERROR_WRITE_FAILED: return "Write failed";
        default: return "Unknown status";
    }
}

allc_string_t allc_process_state_string(allc_process_state_e state) {
    switch (state) {
        case ALLC_PROCESS_STATE_NOT_STARTED: return "Not started";
        case ALLC_PROCESS_STATE_RUNNING: return "Running";
        case ALLC_PROCESS_STATE_FINISHED: return "Finished";
        case ALLC_PROCESS_STATE_TERMINATED: return "Terminated";
        case ALLC_PROCESS_STATE_ERROR: return "Error";
        default: return "Unknown state";
    }
}

#endif // ALLC_PROCESS_IMPL

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker

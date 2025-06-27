/**
 * Tests for os/process.h
 * ======================
 *
 * Comprehensive tests for process management functionality.
 */

#define ALLC_IMPL

#include "../dev/macro.h"
#include "../dev/os/process.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// Basic Process Tests
// ===================

void test__allc_process_creation_deletion() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_system();
    
    // Test basic creation
    allc_process_t proc = allc_process_new(allocator);
    ALLC_TEST_BOOL(true, proc != NULL);
    ALLC_TEST_ANY("-1", "%d", allc_process_get_pid(proc));
    ALLC_TEST_ANY("0", "%d", allc_process_get_state(proc)); // NOT_STARTED
    ALLC_TEST_ANY("0", "%d", allc_process_get_exit_code(proc));
    
    // Test deletion
    allc_process_delete(&proc);
    ALLC_TEST_BOOL(true, proc == NULL);
    
    printf(">> Test creation with null allocator:\n");
    allc_process_t null_proc = allc_process_new((allc_allocator_t){0});
    ALLC_TEST_BOOL(true, null_proc == NULL);
}

void test__allc_process_default_options() {
    printf("\n[%s]\n", __func__);
    
    allc_process_options_t options = allc_process_default_options();
    
    ALLC_TEST_BOOL(false, options.capture_stdout);
    ALLC_TEST_BOOL(false, options.capture_stderr);
    ALLC_TEST_BOOL(true, options.inherit_environment);
    ALLC_TEST_BOOL(true, options.working_directory == NULL);
    ALLC_TEST_ANY("0", "%u", options.timeout_seconds);
}

void test__allc_process_utility_functions() {
    printf("\n[%s]\n", __func__);
    
    // Test current PID
    pid_t current_pid = allc_process_current_pid();
    pid_t getpid_result = getpid();
    ALLC_TEST_BOOL(true, current_pid == getpid_result);
    
    // Test parent PID
    pid_t parent_pid = allc_process_parent_pid();
    pid_t getppid_result = getppid();
    ALLC_TEST_BOOL(true, parent_pid == getppid_result);
    
    // Test process exists
    ALLC_TEST_BOOL(true, allc_process_exists(current_pid));
    ALLC_TEST_BOOL(false, allc_process_exists(999999)); // Very unlikely to exist
    
    printf(">> Test status strings:\n");
    ALLC_TEST_ANY("Success", "%s", allc_process_status_string(ALLC_PROCESS_SUCCESS));
    ALLC_TEST_ANY("Fork failed", "%s", allc_process_status_string(ALLC_PROCESS_ERROR_FORK_FAILED));
    ALLC_TEST_ANY("Unknown status", "%s", allc_process_status_string(999));
    
    printf(">> Test state strings:\n");
    ALLC_TEST_ANY("Not started", "%s", allc_process_state_string(ALLC_PROCESS_STATE_NOT_STARTED));
    ALLC_TEST_ANY("Running", "%s", allc_process_state_string(ALLC_PROCESS_STATE_RUNNING));
    ALLC_TEST_ANY("Unknown state", "%s", allc_process_state_string(999));
}

// Process Execution Tests
// ========================

void test__allc_process_simple_execution() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_system();
    allc_process_t proc = allc_process_new(allocator);
    
    // Test spawning a simple command that should succeed
    char *args[] = {"/bin/true", NULL};
    allc_process_status_e status = allc_process_spawn(&proc, "/bin/true", args, NULL);
    
    ALLC_TEST_ANY("0", "%d", status); // ALLC_PROCESS_SUCCESS
    ALLC_TEST_BOOL(true, allc_process_get_pid(proc) > 0);
    ALLC_TEST_ANY("1", "%d", allc_process_get_state(proc)); // RUNNING
    ALLC_TEST_BOOL(true, allc_process_get_command(proc) != NULL);
    ALLC_TEST_ANY("/bin/true", "%s", allc_process_get_command(proc));
    
    // Wait for completion
    allc_i32_t exit_code = allc_process_wait(&proc);
    ALLC_TEST_ANY("0", "%d", exit_code);
    ALLC_TEST_ANY("2", "%d", allc_process_get_state(proc)); // FINISHED
    ALLC_TEST_BOOL(false, allc_process_is_running(&proc));
    
    allc_process_delete(&proc);
}

void test__allc_process_failing_execution() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_system();
    allc_process_t proc = allc_process_new(allocator);
    
    // Test spawning a command that should fail
    char *args[] = {"/bin/false", NULL};
    allc_process_status_e status = allc_process_spawn(&proc, "/bin/false", args, NULL);
    
    ALLC_TEST_ANY("0", "%d", status); // ALLC_PROCESS_SUCCESS (spawn succeeded)
    
    // Wait for completion
    allc_i32_t exit_code = allc_process_wait(&proc);
    ALLC_TEST_ANY("1", "%d", exit_code); // /bin/false exits with 1
    ALLC_TEST_ANY("2", "%d", allc_process_get_state(proc)); // FINISHED
    
    allc_process_delete(&proc);
}

void test__allc_process_spawn_wait_convenience() {
    printf("\n[%s]\n", __func__);
    
    // Test convenience function for simple commands
    char *args_true[] = {"/bin/true", NULL};
    allc_i32_t exit_code_true = allc_process_spawn_wait("/bin/true", args_true);
    ALLC_TEST_ANY("0", "%d", exit_code_true);
    
    char *args_false[] = {"/bin/false", NULL};
    allc_i32_t exit_code_false = allc_process_spawn_wait("/bin/false", args_false);
    ALLC_TEST_ANY("1", "%d", exit_code_false);
    
    // Test execute function (alias)
    allc_i32_t execute_result = allc_process_execute("/bin/true", args_true);
    ALLC_TEST_ANY("0", "%d", execute_result);
}

void test__allc_process_with_arguments() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_system();
    allc_process_t proc = allc_process_new(allocator);
    
    // Test echo command with arguments
    char *args[] = {"echo", "hello", "world", NULL};
    allc_process_status_e status = allc_process_spawn(&proc, "/bin/echo", args, NULL);
    
    ALLC_TEST_ANY("0", "%d", status);
    
    allc_i32_t exit_code = allc_process_wait(&proc);
    ALLC_TEST_ANY("0", "%d", exit_code);
    
    allc_process_delete(&proc);
}

// Error Handling Tests
// ====================

void test__allc_process_error_conditions() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_system();
    allc_process_t proc = allc_process_new(allocator);
    
    printf(">> Test invalid command:\n");
    char *args[] = {"nonexistent", NULL};
    allc_process_status_e status = allc_process_spawn(&proc, "/nonexistent/command", args, NULL);
    
    // Spawn might succeed (fork works) but exec will fail
    if (status == ALLC_PROCESS_SUCCESS) {
        allc_i32_t exit_code = allc_process_wait(&proc);
        ALLC_TEST_ANY("127", "%d", exit_code); // Standard exec failure code
    }
    
    allc_process_delete(&proc);
    
    printf(">> Test null pointer errors:\n");
    status = allc_process_spawn(NULL, "/bin/true", args, NULL);
    ALLC_TEST_ANY("8", "%d", status); // ALLC_PROCESS_ERROR_NULL_POINTER
    
    proc = allc_process_new(allocator);
    status = allc_process_spawn(&proc, NULL, args, NULL);
    ALLC_TEST_ANY("8", "%d", status); // ALLC_PROCESS_ERROR_NULL_POINTER
    
    allc_process_delete(&proc);
}

void test__allc_process_double_spawn() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_system();
    allc_process_t proc = allc_process_new(allocator);
    
    // First spawn should succeed
    char *args[] = {"/bin/true", NULL};
    allc_process_status_e status1 = allc_process_spawn(&proc, "/bin/true", args, NULL);
    ALLC_TEST_ANY("0", "%d", status1);
    
    // Second spawn should fail (process already started)
    allc_process_status_e status2 = allc_process_spawn(&proc, "/bin/true", args, NULL);
    ALLC_TEST_ANY("1", "%d", status2); // ALLC_PROCESS_ERROR_INVALID_ARGS
    
    allc_process_wait(&proc);
    allc_process_delete(&proc);
}

// Process State Management Tests
// ==============================

void test__allc_process_state_tracking() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_system();
    allc_process_t proc = allc_process_new(allocator);
    
    // Initial state
    ALLC_TEST_ANY("0", "%d", allc_process_get_state(proc)); // NOT_STARTED
    ALLC_TEST_BOOL(false, allc_process_is_running(&proc));
    
    // After spawn
    char *args[] = {"sleep", "0.1", NULL};
    allc_process_spawn(&proc, "/bin/sleep", args, NULL);
    ALLC_TEST_ANY("1", "%d", allc_process_get_state(proc)); // RUNNING
    ALLC_TEST_BOOL(true, allc_process_is_running(&proc));
    
    // After completion
    allc_process_wait(&proc);
    ALLC_TEST_ANY("2", "%d", allc_process_get_state(proc)); // FINISHED
    ALLC_TEST_BOOL(false, allc_process_is_running(&proc));
    
    allc_process_delete(&proc);
}

void test__allc_process_termination() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_system();
    allc_process_t proc = allc_process_new(allocator);
    
    // Start a long-running process
    char *args[] = {"sleep", "10", NULL};
    allc_process_status_e status = allc_process_spawn(&proc, "/bin/sleep", args, NULL);
    ALLC_TEST_ANY("0", "%d", status);
    ALLC_TEST_BOOL(true, allc_process_is_running(&proc));
    
    // Terminate the process
    allc_process_status_e term_status = allc_process_terminate(&proc);
    ALLC_TEST_ANY("0", "%d", term_status);
    
    // Wait and check if it was terminated
    allc_i32_t exit_code = allc_process_wait(&proc);
    ALLC_TEST_BOOL(true, exit_code < 0); // Negative indicates signal termination
    ALLC_TEST_ANY("3", "%d", allc_process_get_state(proc)); // TERMINATED
    
    allc_process_delete(&proc);
}

// Process Options Tests
// =====================

void test__allc_process_with_options() {
    printf("\n[%s]\n", __func__);
    
    allc_allocator_t allocator = allc_allocator_system();
    allc_process_t proc = allc_process_new(allocator);
    
    // Test with capture options
    allc_process_options_t options = allc_process_default_options();
    options.capture_stdout = true;
    options.capture_stderr = true;
    
    char *args[] = {"echo", "test output", NULL};
    allc_process_status_e status = allc_process_spawn_with_options(&proc, "/bin/echo", args, NULL, &options);
    
    ALLC_TEST_ANY("0", "%d", status);
    
    allc_process_wait(&proc);
    
    // Try to read output (this is basic test - real implementation would need buffering)
    char buffer[256];
    allc_size_t bytes_read = allc_process_read_stdout(&proc, buffer, sizeof(buffer) - 1);
    
    // Note: This might be 0 if the process has already finished and pipe is closed
    printf(">> Bytes read from stdout: %zu\n", bytes_read);
    
    allc_process_delete(&proc);
}

// Comprehensive Test Suite
// ========================

void test__allc_process_comprehensive() {
    printf("\n[%s]\n", __func__);
    
    printf(">> Testing multiple processes sequentially:\n");
    
    for (int i = 0; i < 3; i++) {
        allc_allocator_t allocator = allc_allocator_system();
        allc_process_t proc = allc_process_new(allocator);
        
        char *args[] = {"/bin/true", NULL};
        allc_process_spawn(&proc, "/bin/true", args, NULL);
        allc_i32_t exit_code = allc_process_wait(&proc);
        
        ALLC_TEST_ANY("0", "%d", exit_code);
        
        allc_process_delete(&proc);
    }
    
    printf(">> All sequential processes completed successfully\n");
}

// Main Test Runner
// ================

int main() {
    printf("=== Process Module Tests ===\n");
    
    // Basic functionality tests
    test__allc_process_creation_deletion();
    test__allc_process_default_options();
    test__allc_process_utility_functions();
    
    // Process execution tests
    test__allc_process_simple_execution();
    test__allc_process_failing_execution();
    test__allc_process_spawn_wait_convenience();
    test__allc_process_with_arguments();
    
    // Error handling tests
    test__allc_process_error_conditions();
    test__allc_process_double_spawn();
    
    // State management tests
    test__allc_process_state_tracking();
    test__allc_process_termination();
    
    // Advanced functionality tests
    test__allc_process_with_options();
    
    // Comprehensive tests
    test__allc_process_comprehensive();
    
    printf("\n=== All Process Tests Completed ===\n");
    return 0;
}

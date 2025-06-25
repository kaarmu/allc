/***
Kaj Munhoz Arfvidsson, 2023
***/

#define ALLC_IMPL // During development

#ifdef ALLC_IMPL
#   define ALLC_TYPES_IMPL
#   define ALLC_CSTR_IMPL
#   define ALLC_STRBUF_IMPL
#   define ALLC_PROCESS_IMPL
#endif

#include "../types.h"
#include "../cstr.h"
#include "../strbuf.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef ALLC_PROCESS__GUARD
#define ALLC_PROCESS__GUARD

// Macros {{{1
// ============

#define return_(value)  \
    do {                \
        result = value; \
        goto defer;     \
    } while (0)


// Process Management {{{1
// =======================


// }}}1

#endif // ALLC_PROCESS__GUARD

#ifndef ALLC_PROCESS_IMPL__GUARD
#   define ALLC_PROCESS_IMPL__GUARD
#else
#   undef ALLC_PROCESS_IMPL
#endif

#ifdef ALLC_PROCESS_IMPL // {{{1

int allc_process_spawn(char *path, char *argv[])
{
    int result;

    pid_t pid = fork();

    if (pid == 0) {
        // child process
        execve(path, argv, NULL);
        return_(-1); // execve only returns if an error occurred
    } else if (pid == -1) {
        // parent process: fork failed
        return_(-1);
    } else {
        // parent process: fork succeeded
        return_(pid);
    }

defer:
    return result;
}

int allc_process_spawn_wait(char *path, char *argv[])
{
    int pid = allc_process_spawn(path, argv);
    if (pid == -1) {
        return -1;
    }

    // Wait for the child process to finish
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        // The child process exited normally
        return WEXITSTATUS(status);
    } else {
        // The child process was terminated by a signal or something else
        return -1;
    }

    return pid;
}

/*
void exit(int status): Terminates the calling process with the specified exit status.
void _exit(int status): Similar to exit, but does not perform any cleanup operations (like calling functions registered with atexit).
*/

void allc_process_exit(int status);

void allc_process_exit_fast(int status);

#endif // ALLC_PROCESS_IMPL }}}1

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker

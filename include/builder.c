// #include "builder.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

// open for read or write
void allc_builder_open();

void allc_builder_close();

void pid_wait(pid_t pid) {
  for (;;) {
    int wstatus = 0;
    if (waitpid(pid, &wstatus, 0) < 0) {
      // Could not wait on command: pid
      exit(3);
    }

    if (WIFEXITED(wstatus)) {
      int exit_status = WEXITSTATUS(wstatus);
      if (exit_status != 0) {
        // Command exited with exit code: exit_status
        exit(4);
      }

      break;
    }

    if (WIFSIGNALED(wstatus)) {
      // Command process was terminated by strsignal(WTERMSIG(wstatus))
      exit(5);
    }
  }
}

typedef const char *String;

void allc_builder_call(size_t n, char *cmd[static n + 1]) {
  cmd[n] = NULL;

  printf("> ");
  for (size_t i = 0; i < n; ++i) {
    printf(" %s", cmd[i]);
  }
  printf("\n");

  pid_t cpid = fork();
  if (cpid < 0) {
    // Could not fork child process: cmd, errno
    exit(1);
  }

  if (cpid > 0) {
    return;
  }

  if (execvp(cmd[0], cmd) < 0) {
    // Could not exec child process: cmd
    exit(2);
  }

  pid_wait(cpid);
};

void allc_builder_build(char *path, char *flags) {
  allc_builder_call(3, (char *[]){"cc", flags, path});
}

int main() {
  allc_builder_build("cstr.c", "-c -o cstr.o");

  return 0;
}

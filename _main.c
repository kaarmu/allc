#define ALLC_IMPL_BUILDER
#define ALLC_IMPL_DEPENDENCIES
#include "src/builder.h"

#include <ctype.h>

/* Split strbuf into a list of strbuf on whitespace characters. */
List allc_strbuf_split_space(StrBuf *self)
{
    Allocator allocator = allc_allocator();
    List list = allc_list_new(allocator);
    StrBuf temp = allc_strbuf_new(allocator);
    StrBuf *left = &temp, *right = self;
    size_t i;
    while (true) {
        allc_strbuf_strip_left_blank(right);
        i = allc_cstr_find_blank(right->data);
        if (i == right->length) { break; }
        allc_strbuf_split_at(right, i, left, right);
        allc_list_append(&list, sizeof (StrBuf), left);
    }
    allc_strbuf_del(&temp);
    return list;
}

Builder allc_builder_new()
{
    Allocator allocator = allc_allocator();
    Logger logger = {
        .stream = stderr,
        .allocator = allocator,
    };
    return (Builder) {
        .logger = logger,
        .allocator = allocator,
    };
};

void allc_builder_exec(Builder *self, const char *command)
{
    allc_logger_log_info(&self->logger,
                         "Executing \"%s\".",
                         command);
    pid_t cpid = fork();
    if (cpid < 0) {
        allc_logger_panic(&self->logger,
                          "(%s) Could not fork a child process with command \"%s\".",
                          strerror(errno),
                          command);
    }
    if (cpid == 0) {
        StrBuf cmd = allc_strbuf_copy_cstr(command, self->allocator);
        List list = allc_strbuf_split_space(&cmd);
        StrBuf *slice[list.length];
        allc_list_slice(&list, 0, list.length, (void **) slice);
        const char *file = slice[0]->data;
        char *argv[list.length-1];
        allc_logger_log_info(&self->logger, "> %s", file);
        for (size_t i = 1; i < list.length; i++) {
            argv[i] = slice[i]->data;
            allc_logger_log_info(&self->logger, ">> %s", argv[i]);
        }
        if (execvp(file, argv) < 0) {
            allc_logger_panic(&self->logger,
                              "(%s) Could not execute child process with command \"%s\".",
                              strerror(errno),
                              command);
        }
        allc_list_del(&list);
        allc_strbuf_del(&cmd);
    } else {
        while (true) {
            int status;
            if (waitpid(cpid , &status , WNOHANG) < 0) {
                allc_logger_panic(&self->logger,
                                  "An error occurred while waiting for a child process to complete.");
            }
            if (WIFSIGNALED(status) != 0) {
                allc_logger_panic(&self->logger,
                                  "The command \"%s\" was terminated by %s.",
                                  command,
                                  strsignal(WTERMSIG(status)));
            }
            if (WIFEXITED(status) == 0) {
                continue;
            }
            if (WEXITSTATUS(status) != 0) {
                allc_logger_panic(&self->logger,
                                  "The command \"%s\" exited with exit code %d.",
                                  command,
                                  WEXITSTATUS(status));
            }
        }
    }
}

int main()
{
    Builder bob = allc_builder_new();
    allc_builder_exec(&bob, "cat main.c");

}

#define ALLC_IMPL_BUILDER
#define ALLC_IMPL_DEPENDENCIES
#include "src/builder.h"

#include <ctype.h>

size_t  allc_cstr_find_blank(const char *str)
{
    const char *p = str;
    while (*p != 0 && !isblank(*p)) {
        ++p;
    }
    return p-str;
}
void    allc_cstr_replacen_char(char *str, const int n, const char chr, const char *rpl);
void    allc_cstr_replacen_cstr(char *str, const int n, const char *sub, const char *rpl);
void    allc_cstr_replaceall_char(char *str, const char chr, const char *rpl);
void    allc_cstr_replaceall_cstr(char *str, const char *sub, const char *rpl);
void    allc_cstr_capitalize(char *str);
void    allc_cstr_lower(char *str);
void    allc_cstr_upper(char *str);
void    allc_cstr_swap_case(char *str);
void    allc_cstr_rshift(char *str, size_t n)
{}
void    allc_cstr_lshift(char *str, size_t n)
{}

void    allc_strbuf_set_cstr(StrBuf *self, const char *str);
void    allc_strbuf_split_at(StrBuf *self, size_t i, StrBuf *left, StrBuf *right);
void    allc_strbuf_center(StrBuf *str_buf, size_t width, const char chr);
void    allc_strbuf_ljust(StrBuf *str_buf, size_t width, const char chr);
void    allc_strbuf_rjust(StrBuf *str_buf, size_t width, const char chr);
void    allc_strbuf_lstrip_blank(StrBuf *self)
{
    for (const char *p = self->data; *p != 0; ++p) {
        if (isblank(*p)) { continue; }
        allc_strbuf_remove(self, 0, p - self->data);
        return;
    }
}
void    allc_strbuf_rstrip_blank(StrBuf *self)
{
    const char *end = self->data + self->length;
    for (const char *p = end - 1; p != self->data - 1; --p) {
        if (isblank(*p)) { continue; }
        allc_strbuf_remove(self, p+1 - self->data, self->length);
        return;
    }
}
void    allc_strbuf_strip_blank(StrBuf *str_buf)
{
    allc_strbuf_lstrip_blank(str_buf);
    allc_strbuf_rstrip_blank(str_buf);
}
void    allc_strbuf_expand_tabs(StrBuf *str_buf);
void    allc_strbuf_join(StrBuf *str_buf, const char *glue, ...);

/* Split strbuf into a list of strbuf on whitespace characters. */
List allc_strbuf_split_space(StrBuf *self)
{
    Allocator allocator = allc_allocator();
    List list = allc_list_new(allocator);
    StrBuf temp = allc_strbuf_new(0, allocator);
    StrBuf *left = &temp, *right = self;
    size_t i;
    while (true) {
        allc_strbuf_lstrip_blank(right);
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
    pid_t cpid = fork();
    if (cpid < 0) {
        allc_logger_panic(&self->logger,
                          "(%s) Could not fork a child process with command \"%s\".",
                          command,
                          strerror(errno));
    }
    if (cpid == 0) {
        StrBuf cmd = allc_strbuf_copy_cstr(command, self->allocator);
        List list = allc_strbuf_split_space(&cmd);
        StrBuf *slice[list.length];
        allc_list_slice(&list, 0, list.length, (void **) slice);
        const char *file = slice[0]->data;
        char *argv[list.length-1];
        for (size_t i = 1; i < list.length; i++) {
            argv[i] = slice[i]->data;
        }
        if (execvp(file, argv) < 0) {
            allc_logger_panic(&self->logger,
                              "(%s) Could not execute child process with command \"%s\".",
                              command,
                              strerror(errno));
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


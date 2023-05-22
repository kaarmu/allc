#include "../src/macro.h"

#define ALLC_IMPL_STRBUF
#define ALLC_IMPL_DEPENDENCIES
#include "../src/strbuf.h"

void    allc_strbuf_insert_cstr(StrBuf *str_buf, size_t pos, const char *str);
void    allc_strbuf_join_2(StrBuf *self, const char *glue, StrBuf *other)
{
    allc_strbuf_append_cstr(self, glue);
    allc_strbuf_append_cstr(self, other->data);
}
void    allc_strbuf_join_n(StrBuf *self, const char *glue, size_t n, StrBuf **arr)
{
    allc_strbuf_append_cstr(self, (*arr)->data);
    for (StrBuf **p = arr + 1; p != arr + n; ++p) {
        allc_strbuf_append_cstr(self, glue);
        allc_strbuf_append_cstr(self, (*p)->data);
    }
}

void    allc_strbuf_justify_center(StrBuf *str_buf, size_t width, const char chr);
void    allc_strbuf_justify_left(StrBuf *str_buf, size_t width, const char chr);
void    allc_strbuf_justify_right(StrBuf *str_buf, size_t width, const char chr);
void    allc_strbuf_expand_tabs(StrBuf *str_buf);

void    allc_strbuf_insert_cstr(StrBuf *str_buf, size_t pos, const char *str)
{
    size_t length = allc_cstr_length(str);
    allc_strbuf_ensure_capacity(str_buf, str_buf->length + length + 1);
    allc_cstr_shift_right(str_buf->data + pos, length);
    memcpy(str_buf->data + pos, str, length);
    str_buf->length += length;
}

int main()
{
    Allocator allocator = allc_allocator();
    StrBuf str_buf = allc_strbuf_new(allocator);

    allc_strbuf_set_cstr(&str_buf, "Hello, world");
    ALLC_TEST_PRINT("Hello, world :== %s", str_buf.data);
    ALLC_TEST_PRINT("12 :== %zu", str_buf.length);
    ALLC_TEST_PRINT("13 :== %zu", str_buf.capacity);

    allc_strbuf_clear(&str_buf);
    ALLC_TEST_PRINT("0 :== %zu", str_buf.length);
    ALLC_TEST_PRINT("0 :== %zu", allc_cstr_length(str_buf.data));

    allc_strbuf_set_cstr(&str_buf, "Hello, world");
    allc_strbuf_set_cstr(&str_buf, "Hello1 world");
    allc_strbuf_set_cstr(&str_buf, "Hello2 world");
    allc_strbuf_set_cstr(&str_buf, "Hello3 world");
    ALLC_TEST_PRINT("Hello3 world :== %s", str_buf.data);
    ALLC_TEST_PRINT("13 :== %zu", str_buf.capacity);

    allc_strbuf_set_cstr(&str_buf, "Hello, ");
    ALLC_TEST_PRINT("7 :== %zu", str_buf.length);
    allc_strbuf_append_cstr(&str_buf, "world");
    ALLC_TEST_PRINT("12 :== %zu", str_buf.length);
    ALLC_TEST_PRINT("Hello, world :== %s", str_buf.data);
    allc_strbuf_append_cstr(&str_buf, ". I love programming!");
    ALLC_TEST_PRINT("33 :== %zu", str_buf.length);
    ALLC_TEST_PRINT("34 :== %zu", str_buf.capacity);
    ALLC_TEST_PRINT("Hello, world. I love programming! :== %s", str_buf.data);

    allc_strbuf_set_cstr(&str_buf, "Hello, world");
    StrBuf other = allc_strbuf_copy(&str_buf);
    ALLC_TEST_PRINT("Hello, world :== %s", other.data);
    ALLC_TEST_PRINT("12 :== %zu", other.length);
    ALLC_TEST_PRINT("13 :== %zu", other.capacity);
    ALLC_TEST_PRINT("12 :== %zu", str_buf.length);
    ALLC_TEST_PRINT("34 :== %zu", str_buf.capacity);

    allc_strbuf_set_cstr(&str_buf, "Hello");
    allc_strbuf_set_cstr(&other, "world");
    allc_strbuf_join_2(&str_buf, ", ", &other);
    ALLC_TEST_PRINT("world :== %s", other.data);
    ALLC_TEST_PRINT("5 :== %zu", other.length);
    ALLC_TEST_PRINT("Hello, world :== %s", str_buf.data);
    ALLC_TEST_PRINT("12 :== %zu", str_buf.length);

    StrBuf data[] = {
        allc_strbuf_new_cstr(allocator, "For"),
        allc_strbuf_new_cstr(allocator, "the"),
        allc_strbuf_new_cstr(allocator, "Love"),
        allc_strbuf_new_cstr(allocator, "of"),
        allc_strbuf_new_cstr(allocator, "Programming"),
    };
    StrBuf *arr[] = {
        &data[0],
        &data[1],
        &data[2],
        &data[3],
        &data[4],
    };
    allc_strbuf_clear(&str_buf);
    allc_strbuf_join_n(&str_buf, " ", 5, arr);
    ALLC_TEST_PRINT("For the Love of Programming :== %s", str_buf.data);
    ALLC_TEST_PRINT("27 :== %zu", str_buf.length);

    allc_strbuf_split_at(&str_buf, 12, &str_buf, &other);
    ALLC_TEST_PRINT("For the Love :== %s", str_buf.data);
    ALLC_TEST_PRINT("12 :== %zu", str_buf.length);
    ALLC_TEST_PRINT(" of Programming :== %s", other.data);
    ALLC_TEST_PRINT("15 :== %zu", other.length);

    for (StrBuf **s = arr; s != arr + 5; ++s)
        allc_strbuf_del(*s);
    allc_strbuf_del(&str_buf);
    allc_strbuf_del(&other);
}

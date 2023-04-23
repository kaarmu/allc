#include "../src/macro.h"

#define ALLC_IMPL_STRBUF
#define ALLC_IMPL_DEPENDENCIES
#include "../src/strbuf.h"

void allc_strbuf_ensure_capacity(StrBuf *self, size_t n)
{
    if (self->capacity < n) {
        allc_strbuf_grow(self, n - self->capacity);
    }
}
void    allc_strbuf_clear(StrBuf *self)
{
    memset(self->data, 0, self->length);
    self->length = 0;
}
void    allc_strbuf_set_cstr(StrBuf *self, const char *str)
{
    size_t length = allc_cstr_length(str);
    allc_strbuf_ensure_capacity(self, length + 1);
    allc_strbuf_clear(self);
    allc_cstr_copy(str, self->data);
    self->length = length;
}

int main()
{
    Allocator allocator = allc_allocator();
    StrBuf str_buf = allc_strbuf_new(0, allocator);

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

    allc_strbuf_del(&str_buf);
    allc_strbuf_del(&other);
}

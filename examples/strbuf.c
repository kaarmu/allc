#include "../src/macro.h"
#include "../src/strbuf/strbuf.h"

int main()
{
    Allocator allocator = allc_allocator_c();
    StrBuf str_buf = allc_strbuf_new(allocator);

    allc_strbuf_set_cstr(&str_buf, "Hello, world");
    ALLC_TEST("Hello, world",   "%s",   str_buf.data);
    ALLC_TEST("12",             "%zu",  str_buf.length);
    ALLC_TEST("13",             "%zu",  str_buf.capacity);

    allc_strbuf_clear(&str_buf);
    ALLC_TEST("0", "%zu", str_buf.length);
    ALLC_TEST("0", "%zu", allc_cstr_length(str_buf.data));

    allc_strbuf_set_cstr(&str_buf, "Hello, world");
    allc_strbuf_set_cstr(&str_buf, "Hello1 world");
    allc_strbuf_set_cstr(&str_buf, "Hello2 world");
    allc_strbuf_set_cstr(&str_buf, "Hello3 world");
    ALLC_TEST("Hello3 world",   "%s",   str_buf.data);
    ALLC_TEST("13",             "%zu",  str_buf.capacity);

    allc_strbuf_set_cstr(&str_buf, "Hello, ");
    ALLC_TEST("7", "%zu", str_buf.length);
    allc_strbuf_append_cstr(&str_buf, "world");
    ALLC_TEST("12",             "%zu",  str_buf.length);
    ALLC_TEST("Hello, world",   "%s",   str_buf.data);
    allc_strbuf_append_cstr(&str_buf, ". I love programming!");
    ALLC_TEST("33", "%zu", str_buf.length);
    ALLC_TEST("34", "%zu", str_buf.capacity);
    ALLC_TEST("Hello, world. I love programming!",  "%s",   str_buf.data);

    allc_strbuf_set_cstr(&str_buf, "Hello, world");
    StrBuf other = allc_strbuf_copy(&str_buf);
    ALLC_TEST("Hello, world",   "%s",   other.data);
    ALLC_TEST("12", "%zu", other.length);
    ALLC_TEST("13", "%zu", other.capacity);
    ALLC_TEST("12", "%zu", str_buf.length);
    ALLC_TEST("34", "%zu", str_buf.capacity);

    allc_strbuf_set_cstr(&str_buf, "For the Love of Programming");
    allc_strbuf_split_at(&str_buf, 12, &str_buf, &other);
    ALLC_TEST("For the Love",       "%s",   str_buf.data);
    ALLC_TEST("12",                 "%zu",  str_buf.length);
    ALLC_TEST(" of Programming",    "%s",   other.data);
    ALLC_TEST("15",                 "%zu",  other.length);

    allc_strbuf_del(&str_buf);
    allc_strbuf_del(&other);
}

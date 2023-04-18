#include <stdio.h>

#define ALLC_IMPL_CSTR
#include "../src/cstr.h"

int main() {

    printf("0 :== %zu\n", allc_cstr_length(""));
    printf("12 :== %zu\n", allc_cstr_length("Hello, world"));

    printf("true :== %s\n", allc_cstr_repr_bool(allc_cstr_is_digit("123")));
    printf("false :== %s\n", allc_cstr_repr_bool(allc_cstr_is_digit("1s23")));
    printf("false :== %s\n", allc_cstr_repr_bool(allc_cstr_is_alpha("asd341naAd")));
    printf("true :== %s\n", allc_cstr_repr_bool(allc_cstr_is_alnum("asd341naAd")));

    printf("0 :== %zu\n", allc_cstr_find_char("", '!'));
    printf("5 :== %zu\n", allc_cstr_find_char("Hello, world", ','));
    printf("12 :== %zu\n", allc_cstr_find_char("Hello, world", '!'));
    printf("10 :== %zu\n", allc_cstr_findn_char("Hello, world", 3, 'l'));
    printf("12 :== %zu\n", allc_cstr_findn_char("Hello, world", 3, '!'));
    printf("7 :== %zu\n", allc_cstr_find_cstr("Hello, world", "wor"));
    printf("12 :== %zu\n", allc_cstr_find_cstr("Hello, world", "word"));
    printf("12 :== %zu\n", allc_cstr_find_cstr("Hello, world", "words"));

    printf("true :== %s\n", allc_cstr_repr_bool(allc_cstr_is_starting_with("Hello, world", "Hell")));
    printf("false :== %s\n", allc_cstr_repr_bool(allc_cstr_is_starting_with("Hello, world", "Helg")));
    printf("true :== %s\n", allc_cstr_repr_bool(allc_cstr_is_starting_with("Hello, world", "")));

    printf("true :== %s\n", allc_cstr_repr_bool(allc_cstr_is_starting_with("Hello, world", "ld")));
    printf("false :== %s\n", allc_cstr_repr_bool(allc_cstr_is_starting_with("Hello, world", "cold")));
    printf("true :== %s\n", allc_cstr_repr_bool(allc_cstr_is_starting_with("Hello, world", "")));

    char x[13] = "Hello, world";

    allc_cstr_replace_char(x, 'w', 'W');
    printf("Hello, World :== %s\n", x);

    allc_cstr_replace_cstr(x, "Hello", "hyelo");
    printf("hyelo, World :== %s\n", x);

}

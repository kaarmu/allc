#include "../src/macro.h"

#define ALLC_IMPL_CSTR
#include "../src/cstr.h"

void    allc_cstr_capitalize(char *str);
void    allc_cstr_capitalize_all(char *str);
void    allc_cstr_lower(char *str);
void    allc_cstr_lower_all(char *str);
void    allc_cstr_upper(char *str);
void    allc_cstr_upper_all(char *str);
void    allc_cstr_swap_case(char *str);
void    allc_cstr_swap_case_all(char *str);

int main() {

    ALLC_TEST_PRINT("0       :== %zu", allc_cstr_length(""));
    ALLC_TEST_PRINT("12      :== %zu", allc_cstr_length("Hello, world"));

    ALLC_TEST_PRINT("true    :== %s", allc_cstr_repr_bool(allc_cstr_is_digit("123")));
    ALLC_TEST_PRINT("false   :== %s", allc_cstr_repr_bool(allc_cstr_is_digit("1s23")));
    ALLC_TEST_PRINT("false   :== %s", allc_cstr_repr_bool(allc_cstr_is_alpha("asd341naAd")));
    ALLC_TEST_PRINT("true    :== %s", allc_cstr_repr_bool(allc_cstr_is_alnum("asd341naAd")));

    ALLC_TEST_PRINT("0       :== %zu", allc_cstr_find_char("", 1, '!'));
    ALLC_TEST_PRINT("5       :== %zu", allc_cstr_find_char("Hello, world", 1, ','));
    ALLC_TEST_PRINT("12      :== %zu", allc_cstr_find_char("Hello, world", 1, '!'));
    ALLC_TEST_PRINT("10      :== %zu", allc_cstr_find_char("Hello, world", 3, 'l'));
    ALLC_TEST_PRINT("12      :== %zu", allc_cstr_find_char("Hello, world", 3, '!'));
    ALLC_TEST_PRINT("7       :== %zu", allc_cstr_find_cstr("Hello, world", 1, "wor"));
    ALLC_TEST_PRINT("12      :== %zu", allc_cstr_find_cstr("Hello, world", 1, "word"));
    ALLC_TEST_PRINT("12      :== %zu", allc_cstr_find_cstr("Hello, world", 1, "words"));
    ALLC_TEST_PRINT("7       :== %zu", allc_cstr_find_cstr("Hello, world", 1, "wor"));
    ALLC_TEST_PRINT("12      :== %zu", allc_cstr_find_cstr("Hello, world", -1, "word"));
    ALLC_TEST_PRINT("2       :== %zu", allc_cstr_find_cstr("Hello, world", 1, "l"));
    ALLC_TEST_PRINT("10      :== %zu", allc_cstr_find_cstr("Hello, world", -1, "l"));
    ALLC_TEST_PRINT("12      :== %zu", allc_cstr_find_cstr("Hello, world", 3, "!"));
    ALLC_TEST_PRINT("12      :== %zu", allc_cstr_find_cstr("Hello, world", 3, ""));
    ALLC_TEST_PRINT("6       :== %zu", allc_cstr_find_blank("Hello, world"));
    ALLC_TEST_PRINT("12      :== %zu", allc_cstr_find_blank("Hello,.world"));

    ALLC_TEST_PRINT("true    :== %s", allc_cstr_repr_bool(allc_cstr_is_equal("Hello, world", "Hello, world")));
    ALLC_TEST_PRINT("false   :== %s", allc_cstr_repr_bool(allc_cstr_is_equal("Hello, world", "Hello. world")));
    ALLC_TEST_PRINT("false   :== %s", allc_cstr_repr_bool(allc_cstr_is_equal("Hello, world", "Hello, world!")));

    ALLC_TEST_PRINT("true    :== %s", allc_cstr_repr_bool(allc_cstr_is_starting_with("Hello, world", "Hell")));
    ALLC_TEST_PRINT("false   :== %s", allc_cstr_repr_bool(allc_cstr_is_starting_with("Hello, world", "Helg")));
    ALLC_TEST_PRINT("true    :== %s", allc_cstr_repr_bool(allc_cstr_is_starting_with("Hello, world", "")));

    ALLC_TEST_PRINT("true    :== %s", allc_cstr_repr_bool(allc_cstr_is_ending_with("Hello, world", "ld")));
    ALLC_TEST_PRINT("false   :== %s", allc_cstr_repr_bool(allc_cstr_is_ending_with("Hello, world", "cold")));
    ALLC_TEST_PRINT("true    :== %s", allc_cstr_repr_bool(allc_cstr_is_ending_with("Hello, world", "")));

    char x[13];

    allc_cstr_copy("Hello, world", x);
    allc_cstr_replace_char(x, 1, 'w', 'W');
    ALLC_TEST_PRINT("Hello, World     :== %s", x);

    allc_cstr_copy("Hello, world", x);
    allc_cstr_replace_cstr(x, 1, "Hello", "hyelo");
    ALLC_TEST_PRINT("hyelo, world     :== %s", x);

    allc_cstr_copy("Hello, world", x);
    allc_cstr_replace_char(x, 3, 'l', 'i');
    ALLC_TEST_PRINT("Heiio, worid     :== %s", x);

    allc_cstr_copy("Hello, lord", x);
    allc_cstr_replace_cstr(x, 2, "lo", "ya");
    ALLC_TEST_PRINT("Helya, yard      :== %s", x);

    allc_cstr_copy("Hello, lord", x);
    allc_cstr_replace_cstr(x, 3, "lo", "ya");
    ALLC_TEST_PRINT("Helya, yard      :== %s", x);

    allc_cstr_copy("lolo, lolo", x);
    allc_cstr_replace_cstr(x, 2, "lolo", "yoyo");
    ALLC_TEST_PRINT("yoyo, yoyo       :== %s", x);

    allc_cstr_copy("lolo, lolo", x);
    allc_cstr_replace_cstr(x, 2, "lo", "yo");
    ALLC_TEST_PRINT("yoyo, lolo       :== %s", x);

    allc_cstr_copy("lolo, lolo", x);
    allc_cstr_replace_cstr(x, 4, "lo", "yo");
    ALLC_TEST_PRINT("yoyo, yoyo       :== %s", x);

    allc_cstr_copy("Hello, world", x);
    allc_cstr_shift_left(x, 2);
    ALLC_TEST_PRINT("llo, world       :== %s", x);

    allc_cstr_copy("Hello, world", x);
    allc_cstr_shift_right(x, 2);
    ALLC_TEST_PRINT("Hello, wor       :== %s", x);

    allc_cstr_copy("Hello, world", x);
    allc_cstr_remove_suffix(x, "world");
    ALLC_TEST_PRINT("Hello,           :== %s", x);

    allc_cstr_copy("Hello, world", x);
    allc_cstr_remove_prefix(x, "Hello");
    ALLC_TEST_PRINT(", world          :== %s", x);

    allc_cstr_copy("Right blanks  ", x);
    allc_cstr_strip_right_blank(x);
    ALLC_TEST_PRINT("'Right blanks'   :== '%s'", x);

    allc_cstr_copy("  Left blanks", x);
    allc_cstr_strip_left_blank(x);
    ALLC_TEST_PRINT("'Left blanks'    :== '%s'", x);

    allc_cstr_copy("  LR blanks  ", x);
    allc_cstr_strip_blank(x);
    ALLC_TEST_PRINT("'LR blanks'      :== '%s'", x);
}

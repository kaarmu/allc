#include "src/list/_listitem.h"
#include <stdio.h>

int main() {
    printf("%ld\n", sizeof (ListItem*));
    printf("%ld\n", sizeof (ListItem*));
    printf("%ld\n", sizeof (size_t));
    printf("%ld\n", sizeof (ListItem)
                    - sizeof (size_t)
                    - sizeof (ListItem*) * 2);
    printf("%ld\n", sizeof (ListItem));
}

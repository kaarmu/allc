
#include "../src/list/list.h"
#include <stdio.h>

#define PRINT_INFO(list) \
    printf("\n>length=%lu\n\n", (list)->length)

int main()
{

    List list = allc_list_new(allc_allocator_c());

    typedef const char * String;

    {
        String s[4] = {
            "Hello", "Mr.",
            "Kaj", "Arfvidsson",
        };
        allc_list_append(&list, sizeof (String), &s[0]);
        allc_list_append(&list, sizeof (String), &s[1]);
        allc_list_append(&list, sizeof (String), &s[2]);
        allc_list_append(&list, sizeof (String), &s[3]);
    }

    ALLC_LIST_FOREACH(String, elem, &list, {
        printf("%s ", *elem);
    });
    PRINT_INFO(&list);

    {
        String s = "Munhoz";
        allc_list_insert(&list, 3, sizeof (String), &s);
    }

    ALLC_LIST_FOREACH(String, elem, &list, {
        printf("%s ", *elem);
    });
    PRINT_INFO(&list);

    {
        allc_list_remove(&list, 1);
    }

    ALLC_LIST_FOREACH(String, elem, &list, {
        printf("%s ", *elem);
    });
    PRINT_INFO(&list);

    allc_list_del(&list);

    return 0;
}

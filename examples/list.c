
#define ALLC_LIST_SAFE
#define ALLC_IMPL_LIST
#define ALLC_IMPL_DEPENDENCIES

#include "../src/list.h"

int main()
{

    List list = allc_list_new(allc_allocator());

    typedef const char * String;

    {
        size_t n = 4;
        String *argv[n];
        allc_list_create_n(&list, n, sizeof (String), (void **) argv);

        *argv[0] = "Hello";         // 0 -4
        *argv[1] = "Kaj";           // 1 -3
        *argv[2] = "Munhoz";        // 2 -2
        *argv[3] = "Arfvidsson";    // 3 -1
    }

    ALLC_LIST_FOREACH(String, item, &list, {
        if (*item != NULL)
            printf("%s\n", *item);
    });

    printf("\n");

    {
        ListItem *item;
        item = allc_list_item_at(&list, 0);
        printf("0: %p %s\n", item, *((String *) item->data));
        item = allc_list_item_at(&list, 1);
        printf("1: %p %s\n", item, *((String *) item->data));
        item = allc_list_item_at(&list, 2);
        printf("2: %p %s\n", item, *((String *) item->data));
        item = allc_list_item_at(&list, 3);
        printf("3: %p %s\n", item, *((String *) item->data));

        printf("\n");

        item = allc_list_item_at(&list, -1);
        printf("-1: %p %s\n", item, *((String *) item->data));
        item = allc_list_item_at(&list, -2);
        printf("-2: %p %s\n", item, *((String *) item->data));
        item = allc_list_item_at(&list, -3);
        printf("-3: %p %s\n", item, *((String *) item->data));
        item = allc_list_item_at(&list, -4);
        printf("-4: %p %s\n", item, *((String *) item->data));
    }

    printf("\n");

    if (0) {
        allc_list_remove_n(&list, 0, 2);

        size_t n = 2;
        String *argv[n];
        allc_list_create_n(&list, n, sizeof (String), (void **) argv);

        *argv[0] = "is";
        *argv[1] = "GOAT";
    }

    ALLC_LIST_FOREACH(String, item, &list, {
        if (*item != NULL)
            printf("%s\n", *item);
    });

    printf("\n");

    String *p;
    allc_list_create_1(&list, 5, (void **) &p);
    *p = "GOAT";

    ALLC_LIST_FOREACH(String, item, &list, {
        if (*item != NULL)
            printf("%s\n", *item);
    });

    ListItem *x[2] = { 0 };
    allc_list_item_slice(&list, 1, list.length, (void**) x);

    for (int i = 0; i < 2; i++)
        printf(":: %s\n", x[i]->data);

    allc_list_del(&list);

    return 0;
}

/*
 *  Simple doubly linked list.
 *
 *  Description
 *  ===========
 *  << What is this module about? >>
 *
 *  We differentiate between list, item and element in the following way.
 *    - list: an abstract construct that is represented by `List` (a handle that
 *      the user can interact with)
 *    - item: The actual object (`ListItem`) which is allocated. This object
 *      contains pointers to the next and previous items in the list. The user's
 *      element is part of `ListItem` struct.
 *    - element: The user's data/object. The user can interact with the elements
 *      through pointers provided by the `allc_list_*` functions.
 *
 *  Options
 *  =======
 *  << What compilation options is available? >>
 *
 *  ALLC_LIST_SAFE: Apply bound checks and similar.
 *
 *  Content
 *  =======
 *  << What types/subroutines does this module provide? >>
 *
 *  List and List Item
 *  ------------------
 *  [x] ListItem
 *  [x] allc_listitem_skip
 *  [x] List
 *  [x] allc_list_new
 *  [x] allc_list_del
 *
 *  Create and Remove Items
 *  -----------------------
 *  [x] allc_list_item_new
 *  [x] allc_list_item_del
 *  [x] allc_list_create_1
 *  [x] allc_list_create_n
 *  [x] allc_list_remove_1
 *  [x] allc_list_remove_n
 *
 *  Accessing List Content
 *  ----------------------
 *  [x] allc_list_at
 *  [x] allc_list_slice
 *  [x] allc_list_item_at
 *  [x] allc_list_item_slice
 *
 *  Element Operations
 *  ------------------
 *  [ ] allc_list_move
 *  [ ] allc_list_swap
 *  [ ] allc_list_copy_1
 *  [ ] allc_list_copy_n
 *  [ ] allc_list_pop_1
 *  [ ] allc_list_pop_n
 *
 *  List Operations
 *  ---------------
 *  [x] allc_list_append
 *  [ ] allc_list_insert
 *  [ ] allc_list_extend
 *  [ ] allc_list_reverse
 *
 *  Macros
 *  ------
 *  [x] ALLC_LIST_FOREACH
 *  [x] ALLC_LIST_ITEM_FOREACH
 *  [x] ALLC_LIST_DEBUG
 */

#ifndef ALLC_LIST__GUARD
#define ALLC_LIST__GUARD

#include <sys/types.h>

#include "macro.h"
#include "allocator.h"

// List and List Item {{{

/*
 * An item in the list.
 *
 * This is a wrapper around every element in the list. This is why it is important to manage
 * elements only with the provided `allc_list_*` functions, otherwise the list might behave
 * unexpectedly.
 *
 */
typedef struct ListItem {
    struct ListItem *next;
    struct ListItem *prev;
    size_t size;
    unsigned char data[];
} ListItem;

/* Skip forward or backward N steps in the list. */
ListItem *allc_listitem_skip(ListItem *self, ssize_t n);

typedef struct {
    size_t length;
    ListItem *head;
    ListItem *last;
    Allocator allocator;
} List;

/*
 * Create a new, empty list.
 *
 * The list must be freed with `allc_list_del` once it is no longer used.
 */
List allc_list_new(Allocator allocator);

/* Delete the list and free all its items. */
void allc_list_del(List *self);

// Create and Remove Items {{{

/* Allocate a new item. */
ListItem *allc_list_item_new(List *self, size_t size);

/* Delete the item. */
void allc_list_item_del(List *self, ListItem *p);

/*
 * Create 1 item at the given index.
 *
 * The element will be uninitialized. This can be done using the `out` pointer
 * which points to the element.
 *
 * Example:
 * ```
 * MyThing *arr[n];
 * allc_list_create_n(&list, sizeof (MyThing), n, (void **) arr);
 * *arr[0] = { 0 }; // initialize as you want
 * ```
 */
void allc_list_create_1(List *self, size_t size, void **out);

/* Same as `allc_list_create_1` but for N items. */
void allc_list_create_n(List *self, size_t n, size_t size, void **out);

/*
 * Remove 1 item from the list at given index.
 *
 * Consider the list `[A, B, C, D], after calling this function with `i=2` the list will be
 * `[A, B, D]`. If the index is negative then it count backwards through the list, i.e. `i=-1` for
 * D, `i=-2` for C, etc.
 *
 * When removing an item the allocated memory for the item is freed. That means the user must make
 * sure the element is ready to be destroyed before calling this function.
 */
void allc_list_remove_1(List *self, ssize_t i);

/* Same as `allc_list_remove_1` but for N items. */
void allc_list_remove_n(List *self, ssize_t i, size_t n);

// Accessing List Content {{{

/*
 * Get a pointer to the element at given index.
 *
 * Remember, since `List` is agnostic to the type of object inside it every
 * interface to the list will use pointers. In this case, consider a list
 * containing an object of type `MyThing` then `allc_list_at` will return a
 * pointer `MyThing*`.
 */
void *allc_list_at(List *self, ssize_t i);

/* Get a slice (array) of pointers to selected elements. */
void allc_list_slice(List *self, ssize_t i, ssize_t j, void **out);

/*
 * Get a pointer to the item at given index.
 */
ListItem *allc_list_item_at(List *self, ssize_t i);

/* Get a slice (array) of pointers to selected items. */
void allc_list_item_slice(List *self, ssize_t i, ssize_t j, void **out);

// Element Operations {{{

void allc_list_move(List *self, ssize_t i, ssize_t j);
void allc_list_swap(List *self, ssize_t i, ssize_t j);

void allc_list_copy_1(List *self, size_t i, void **out);
void allc_list_copy_n(List *self, size_t i, size_t n, void **out);

void allc_list_pop_1(List *self, ssize_t i);
void allc_list_pop_n(List *self, ssize_t i, size_t n);

// List Operations {{{

/* Add element to the end of the list. */
void allc_list_append(List *self, size_t size, void *elem);

void allc_list_insert(List *self, ssize_t i, size_t size, void *elem);

void allc_list_extend(List *self, List *other);

void allc_list_reverse(List *self);

// Macros {{{

#define ALLC_LIST_FOREACH(T, elem, list, body)                       \
    do {                                                             \
        for (ListItem *p = (list)->head; p != NULL; p = p->next) {   \
            T *elem = (T*) p->data;                                  \
            body;                                                    \
        }                                                            \
    } while(0)

#define ALLC_LIST_ITEM_FOREACH(item, list, body)                     \
    do {                                                             \
        ListItem *item;                                              \
        for (item = (list)->head; item != NULL; item = item->next) { \
            body;                                                    \
        }                                                            \
    } while(0)

#define ALLC_LIST_DEBUG(list)                                                  \
    do {                                                                       \
        printf("[%p, %p]: %zu\n", (list)->last, (list)->head, (list)->length); \
        ALLC_LIST_ITEM_FOREACH(item, (list), {                                 \
            printf("%p: %p, %p\n", item, item->prev, item->next);              \
        });                                                                    \
    } while(0)

#endif // ALLC_LIST__GUARD

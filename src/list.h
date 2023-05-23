/**
 *  Simple doubly linked list.
 *
 *  Description
 *  ===========
 *
 *  Options
 *  =======
 *
 *  ALLC_LIST_SAFE: Apply bound checks and similar.
 *
 *  Content
 *  =======
 *
 *  1) List
 *
 *      [x] List
 *      [x] allc_list_new
 *      [x] allc_list_del
 *
 *      1.1) List Operations
 *
 *          [x] allc_list_append
 *          [ ] allc_list_insert
 *          [ ] allc_list_extend
 *          [ ] allc_list_reverse
 *
 *      1.2) Element Operations
 *
 *          [x] allc_list_at
 *          [ ] allc_list_move
 *          [ ] allc_list_swap
 *          [ ] allc_list_copy
 *          [ ] allc_list_pop
 *          [ ] allc_list_iter_forw
 *          [ ] allc_list_iter_back
 *
 *  2) Macros
 *
 *      [x] ALLC_LIST_FOREACH
 *      [x] ALLC_LIST_DEBUG
 *
 *  (Kaj Munhoz Arfvidsson, 2023)
 **/

#ifndef ALLC_LIST__GUARD
#define ALLC_LIST__GUARD

// Includes {{{1
// -------------

#include <sys/types.h>

#include "allocator.h"


// List {{{1
// ---------

#ifndef ALLC_LISTITEM__GUARD
typedef void ListItem;
#endif // ALLC_LISTITEM__GUARD

typedef struct List {
    size_t length;
    ListItem *head;
    ListItem *last;
    Allocator allocator;
} List;

/**
 *  Create a new, empty list.
 *
 *  The list must be freed with `allc_list_del` once it is no longer used.
 **/
extern List allc_list_new(Allocator allocator);

/* Delete the list and free all its items. */
extern void allc_list_del(List *self);


// List Operations {{{2
// --------------------

/* Add element to the end of the list. */
extern void allc_list_append(List *self, size_t size, void *elem);

extern void allc_list_insert(List *self, ssize_t i, size_t size, void *elem);

extern void allc_list_extend(List *self, List *other);

extern void allc_list_reverse(List *self);


// Element Operations {{{2
// -----------------------

/**
 *  Create 1 item at the given index.
 *
 *  The element will be uninitialized. This can be done using the `out` pointer
 *  which points to the element.
 *
 *  Example
 *  -------
 *  ```
 *  MyThing *arr;
 *  allc_list_create_n(&list, sizeof (MyThing), n, (void **) &arr);
 *  *arr = { 0 }; // initialize as you want
 *  ```
 **/
extern void allc_list_create_1(List *self, size_t size, void **out);

/**
 *  Same as `allc_list_create_1` but for N items.
 *
 *  Example
 *  -------
 *  ```
 *  MyThing *arr[n];
 *  allc_list_create_n(&list, sizeof (MyThing), n, (void **) arr);
 *  *arr[0] = { 0 }; // initialize as you want
 *  ```
 **/
extern void allc_list_create_n(List *self, size_t n, size_t size, void **out);

/**
 *  Remove 1 item from the list at given index.
 *
 *  Consider the list `[A, B, C, D], after calling this function with `i=2` the
 *  list will be `[A, B, D]`. If the index is negative then it count backwards
 *  through the list, i.e. `i=-1` for D, `i=-2` for C, etc.
 *
 *  When removing an item the allocated memory for the item is freed. That means
 *  the user must make sure the element is ready to be destroyed before calling
 *  this function.
 **/
extern void allc_list_remove_1(List *self, ssize_t i);

/* Same as `allc_list_remove_1` but for N items. */
extern void allc_list_remove_n(List *self, ssize_t i, size_t n);

extern void allc_list_copy_1(List *self, size_t i, void **out);
extern void allc_list_copy_n(List *self, size_t i, size_t n, void **out);

/**
 * Get a pointer to the element at given index.
 *
 * Remember, since `List` is agnostic to the type of object inside it every
 * interface to the list will use pointers. In this case, consider a list
 * containing an object of type `MyThing` then `allc_list_at` will return a
 * pointer `MyThing*`.
 **/
extern void *allc_list_at(List *self, ssize_t i);

extern void allc_list_move(List *self, ssize_t i, ssize_t j);
extern void allc_list_swap(List *self, ssize_t i, ssize_t j);

extern void allc_list_pop(List *self, ssize_t i);


// Macros {{{1
// -----------

#define ALLC_LIST_FOREACH(T, elem, list, body)      \
    do {                                            \
        ListItem *item = (list)->head;              \
        if (item != NULL) {                         \
            T *elem = (T*) item->data;              \
            do {                                    \
                body;                               \
                elem = allc_list_iter_forw(elem);   \
            } while(elem != NULL);                  \
        }                                           \
    } while(0)

#define ALLC_LIST_DEBUG(list)                                                  \
    do {                                                                       \
        printf("[%p, %p]: %zu\n", (list)->last, (list)->head, (list)->length); \
        ALLC_LIST_ITEM_FOREACH(item, (list), {                                 \
            printf("%p: %p, %p\n", item, item->prev, item->next);              \
        });                                                                    \
    } while(0)


#endif // ALLC_LIST__GUARD

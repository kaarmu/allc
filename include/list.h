/*
 *  Simple doubly linked list.
 *
 *  Description
 *  ===========
 *  << What is this module about? >>
 *
 *  We differentiate between list, item and element in the following way.
 *    - list: an abstract construct that is represented by `AllCList` (a handle that
 *      the user can interact with)
 *    - item: The actual object (`_AllCListItem`) which is allocated. This object
 *      contains pointers to the next and previous items in the list. The user's
 *      element is part of `_AllCListItem` struct.
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
 *  List
 *  ----
 *  [x] AllCList
 *  [x] allc_list_new
 *  [x] allc_list_del
 *
 *  Create and Remove Items
 *  -----------------------
 *  [x] allc_list_create_1
 *  [x] allc_list_create_n
 *  [x] allc_list_remove_1
 *  [x] allc_list_remove_n
 *
 *  Accessing AllCList Content
 *  ----------------------
 *  [x] allc_list_at
 *  [x] allc_list_slice
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
 *  AllCList Operations
 *  ---------------
 *  [x] allc_list_append
 *  [ ] allc_list_insert
 *  [ ] allc_list_extend
 *  [ ] allc_list_reverse
 *
 *  Macros
 *  ------
 *  [x] ALLC_LIST_FOREACH
 *  [x] ALLC_LIST_DEBUG
 */

#ifndef ALLC_LIST__GUARD
#define ALLC_LIST__GUARD

#include <sys/types.h>

#include "macro.h"
#include "allocator.h"

// List {{{

typedef struct {
    size_t length;
    _AllCListItem *head;
    _AllCListItem *last;
    Allocator allocator;
} AllCList;

/*
 * Create a new, empty list.
 *
 * The list must be freed with `allc_list_del` once it is no longer used.
 */
AllCList allc_list_new(Allocator allocator);

/* Delete the list and free all its items. */
void allc_list_del(AllCList *self);

// Create and Remove Items {{{

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
void allc_list_create_1(AllCList *self, size_t size, void **out);

/* Same as `allc_list_create_1` but for N items. */
void allc_list_create_n(AllCList *self, size_t n, size_t size, void **out);

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
void allc_list_remove_1(AllCList *self, ssize_t i);

/* Same as `allc_list_remove_1` but for N items. */
void allc_list_remove_n(AllCList *self, ssize_t i, size_t n);

// Accessing AllCList Content {{{

/*
 * Get a pointer to the element at given index.
 *
 * Remember, since `AllCList` is agnostic to the type of object inside it every
 * interface to the list will use pointers. In this case, consider a list
 * containing an object of type `MyThing` then `allc_list_at` will return a
 * pointer `MyThing*`.
 */
void *allc_list_at(AllCList *self, ssize_t i);

/* Get a slice (array) of pointers to selected elements. */
void allc_list_slice(AllCList *self, ssize_t i, ssize_t j, void **out);

// Element Operations {{{

void allc_list_move(AllCList *self, ssize_t i, ssize_t j);
void allc_list_swap(AllCList *self, ssize_t i, ssize_t j);

void allc_list_copy_1(AllCList *self, size_t i, void **out);
void allc_list_copy_n(AllCList *self, size_t i, size_t n, void **out);

void allc_list_pop_1(AllCList *self, ssize_t i);
void allc_list_pop_n(AllCList *self, ssize_t i, size_t n);

// AllCList Operations {{{

/* Add element to the end of the list. */
void allc_list_append(AllCList *self, size_t size, void *elem);

void allc_list_insert(AllCList *self, ssize_t i, size_t size, void *elem);

void allc_list_extend(AllCList *self, AllCList *other);

void allc_list_reverse(AllCList *self);

// Macros {{{

#define ALLC_LIST_FOREACH(T, elem, list, body)                       \
    do {                                                             \
        for (_AllCListItem *p = (list)->head; p != NULL; p = p->next) {   \
            T *elem = (T*) p->data;                                  \
            body;                                                    \
        }                                                            \
    } while(0)

#define ALLC_LIST_ITEM_FOREACH(item, list, body)                     \
    do {                                                             \
        _AllCListItem *item;                                              \
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

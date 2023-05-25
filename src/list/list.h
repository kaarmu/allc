/**
 *  Simple doubly linked list.
 *
 *  Description
 *  ===========
 *
 *  Options
 *  =======
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
 *          [x] allc_list_remove
 *          [x] allc_list_iter_forw
 *          [x] allc_list_iter_back
 *
 *  2) Macros
 *
 *      [x] ALLC_LIST_FOREACH
 *
 *  (Kaj Munhoz Arfvidsson, 2023)
 **/

#ifndef ALLC_LIST__GUARD
#define ALLC_LIST__GUARD

// Includes {{{1
// -------------

#include "../allocator/allocator.h"

#include <sys/types.h>


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

/* Insert element at the given index. */
extern void allc_list_insert(List *self, ssize_t i, size_t size, void *elem);

extern void allc_list_extend(List *self, List *other);

extern void allc_list_reverse(List *self);


// Element Operations {{{2
// -----------------------

/**
 * Get a pointer to the element at given index.
 *
 * Remember, since `List` is agnostic to the type of object inside it every
 * interface to the list will use pointers. In this case, consider a list
 * containing an object of type `MyThing` then `allc_list_at` will return a
 * pointer `MyThing*`.
 *
 * If `self` is NULL then return NULL.
 **/
extern void *allc_list_at(List *self, ssize_t i);

extern void allc_list_move(List *self, ssize_t i, ssize_t j);
extern void allc_list_swap(List *self, ssize_t i, ssize_t j);

/* Remove the element at the given index. */
extern void allc_list_remove(List *self, ssize_t i);

/* Iterate over the elements in the list. */
extern void *allc_list_iter_forw(void *);
extern void *allc_list_iter_back(void *);


// Macros {{{1
// -----------

#define ALLC_LIST_FOREACH(T, elem, list, body)  \
    do {                                        \
        T *elem = (T*) allc_list_at(list, 0);   \
        while(elem != NULL) {                   \
            body;                               \
            elem = allc_list_iter_forw(elem);   \
        }                                       \
    } while(0)


#endif // ALLC_LIST__GUARD

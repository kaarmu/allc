/*
 *  STB-style header library of a very simple (double) linked list
 *  implementation.
 *
 *  Description
 *  ===========
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
 *  Provides
 *  ========
 *
 *  ListItem
 *
 *  List
 *
 *  Options
 *  =======
 *
 *  ALLC_LIST_SAFE: Apply bound checks and similar.
 *
 *  Library
 *  =======
 *
 *  Dependencies:
 *    - macro.h
 *    - allocator.h
 *
 *  Version: 0.0.0
 *
 *  Author: Kaj Munhoz Arfvidsson, 2023
 */

#ifndef ALLC_LIST__GUARD // {{{1
#define ALLC_LIST__GUARD

#include <sys/types.h>

#include "macro.h"
#include "allocator.h"

// ListItem {{{2

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

// List {{{2

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

// Create and Remove Items {{{3

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

// Accessing List Content {{{3

/*
 * Get a pointer to the item at given index.
 */
ListItem *allc_list_item_at(List *self, ssize_t i);

/*
 * Get a pointer to the element at given index.
 *
 * Remember, since `List` is agnostic to the type of object inside it every
 * interface to the list will use pointers. In this case, consider a list
 * containing an object of type `MyThing` then `allc_list_at` will return a
 * pointer `MyThing*`.
 */
void *allc_list_at(List *self, ssize_t i);

/* Get a slice (array) of pointers to selected items. */
void allc_list_item_slice(List *self, ssize_t i, ssize_t j, void **out);

/* Get a slice (array) of pointers to selected elements. */
void allc_list_slice(List *self, ssize_t i, ssize_t j, void **out);

// Element Operations {{{3

void allc_list_move(List *self, ssize_t i, ssize_t j);
void allc_list_swap(List *self, ssize_t i, ssize_t j);

void allc_list_copy_1(List *self, size_t i, void **out);
void allc_list_copy_n(List *self, size_t i, size_t n, void **out);

void allc_list_pop_1(List *self, ssize_t i);
void allc_list_pop_n(List *self, ssize_t i, size_t n);

// Append and Insert {{{3

void allc_list_append(List *self, size_t size, void *elem);
void allc_list_insert(List *self, size_t size, void *elem);

// List Operations {{{3

void allc_list_extend(List *self, List *other);
void allc_list_reverse(List *self);

// Macros {{{2

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

#endif // ALLC_LIST__GUARD }}}1

#ifdef ALLC_IMPL_LIST // {{{1
#ifdef ALLC_IMPL_DEPENDENCIES
#   define ALLC_IMPL_ALLOCATOR
#endif
#ifndef ALLC_IMPL_LIST__GUARD
#define ALLC_IMPL_LIST__GUARD

#include <stdio.h>
#include <string.h>

#include "allocator.h"

ListItem *allc_listitem_skip(ListItem *self, ssize_t n)
{
    ListItem *p = self;
    if (n < 0) {
        for (; n != 0; n++) {
#ifdef ALLC_LIST_SAFE
            if (p->prev == NULL)
                ALLC_PANIC("Skipping past end of list.\n");
#endif
            p = p->prev;
        }
    } else {
        for (; n != 0; n--) {
#ifdef ALLC_LIST_SAFE
            if (p->next == NULL)
                ALLC_PANIC("Skipping past end of list.\n");
#endif
            p = p->next;
        }
    }
    return p;
}

List allc_list_new(Allocator allocator)
{
    return (List) {
        .length = 0,
        .head = NULL,
        .last = NULL,
        .allocator = allocator,
    };
}

void allc_list_del(List *self)
{
    for (ListItem *q, *p = self->head; p != NULL; p = q) {
        q = p->next;
        allc_list_item_del(self, p);
    }
}

ListItem *allc_list_item_new(List *self, size_t size)
{
    ListItem *p = self->allocator.alloc(size + sizeof (ListItem));
    p->size = size;
    return p;
}

void allc_list_item_del(List *self, ListItem *p)
{
    self->allocator.free(p);
}

void allc_list_create_1(List *self, size_t size, void **out)
{
    ListItem *p = allc_list_item_new(self, size);
    p->prev = self->last;
    if (self->length == 0) {
        self->head = p;
        self->last = p;
    } else {
        self->last->next = p;
        self->last = p;
    }
    self->length += 1;
    *out = p->data;
}

void allc_list_create_n(List *self, size_t n, size_t size, void **out)
{
    if (self->length == 0) {
        allc_list_create_1(self, size, (void**) &out[0]);
        allc_list_create_n(self, n-1, size, (void**) &out[1]);
        return;
    }
    ListItem *arr[n];
    arr[0] = allc_list_item_new(self, size);
    out[0] = arr[0]->data;
    for (size_t i = 1; i < n; i++) {
        arr[i] = allc_list_item_new(self, size);
        arr[i]->prev = arr[i-1];
        arr[i-1]->next = arr[i];
        out[i] = arr[i]->data;
    }
    arr[0]->prev = self->last;
    self->last->next = arr[0];
    self->last = arr[n-1];
    self->length += n;
}

void allc_list_remove_1(List *self, ssize_t i)
{
    ListItem *p = allc_list_item_at(self, i);
    if (p->prev != NULL) { p->prev->next = p->next; }
    if (p->next != NULL) { p->next->prev = p->prev; }
    if (p == self->head) { self->head = p->next; }
    if (p == self->last) { self->last = p->prev; }
    self->length -= 1;
    self->allocator.free(p);
}

void allc_list_remove_n(List *self, ssize_t i, size_t n)
{
#ifdef ALLC_LIST_SAFE
    if (!(i+n < self->length))
        ALLC_PANIC("Trying to remove elements outside of bounds.\n");
    // NOTE: we really want to test |n-i| here but since n is always positive this is also true
    if (!(n-i < self->length))
        // This can also happen if you try to remove [-1..3] in a 4-item list
        ALLC_PANIC("Trying to remove more elements than in list.\n");
#endif
    ListItem *arr[n];
    arr[0] = allc_list_item_at(self, i);
    for (size_t i = 1; i < n; i++) {
        arr[i] = arr[i-1]->next;
    }
    if (arr[0]->prev != NULL)   { arr[0]->prev->next = arr[n-1]->next; }
    if (arr[n-1]->next != NULL) { arr[n-1]->next->prev = arr[0]->prev; }
    if (arr[0] == self->head)   { self->head = arr[n-1]->next; }
    if (arr[n-1] == self->last) { self->last = arr[n]->prev; }
    self->length -= n;
    for (size_t i = 0; i < n; i++) {
        self->allocator.free(arr[i]);
    }
}

ListItem *allc_list_item_at(List *self, ssize_t i)
{
#ifdef ALLC_LIST_SAFE
    ssize_t length = self->length;
    if (!(-length <= i && i < length))
        ALLC_PANIC("Index out of bounds.\n");
#endif
    ListItem *p;
    if (i < 0) {
        i += 1;
        p = self->last;
    } else {
        p = self->head;
    }
    return allc_listitem_skip(p, i);
}

void *allc_list_at(List *self, ssize_t i)
{
    ListItem *p = allc_list_item_at(self, i);
    return p->data;
}

void allc_list_item_slice(List *self, ssize_t i, ssize_t j, void **out)
{
    ssize_t length = self->length;
    if (i < 0) { i += length; }
    if (j < 0) { j += length; }
    if (i == j) { return; }
#ifdef ALLC_LIST_SAFE
    if (j - i < 0)
        ALLC_PANIC("Invalid use of indices.\n");
    if (j - length < 0)
        ALLC_PANIC("Index out of bounds.\n");
#endif
    ListItem *item;
    out[0] = item = allc_list_item_at(self, i);
    for (ssize_t k = 1; k < j-i; k++) {
        out[k] = item = item->next;
    }
}

void allc_list_slice(List *self, ssize_t i, ssize_t j, void **out)
{
    if (i < 0) { i += self->length; }
    if (j < 0) { j += self->length; }
    if (i == j) { return; }
#ifdef ALLC_LIST_SAFE
    if (j - i < 0)
        ALLC_PANIC("Invalid use of indices.\n");
#endif
    ListItem *item;
    item = allc_list_item_at(self, i);
    out[0] = item->data;
    for (ssize_t k = 1; k < j-i; k++) {
        item = item->next;
        out[k] = item->data;
    }
}

#endif // ALLC_IMPL_LIST__GUARD
#endif // ALLC_IMPL_LIST }}}1


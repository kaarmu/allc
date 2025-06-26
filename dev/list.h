/**
 *  Simple doubly linked list.
 *
 *  Description
 *  ===========
 *
 *  A generic doubly-linked list implementation that stores elements of any type.
 *  Each element is stored with a size, making the list type-agnostic.
 *  Built on the allocator abstraction for custom memory management.
 *
 *  Options
 *  =======
 *
 *  #define ALLC_IMPL before including to get the implementation.
 *
 *  (Authored by Kaj Munhoz Arfvidsson, 2023)
 **/

#ifndef ALLC_LIST__GUARD
#define ALLC_LIST__GUARD

// Includes {{{1
// =============

#include "allocator.h"
#include <string.h>
#include <sys/types.h>

// Macros {{{1
// ===========

#ifdef ALLC_IMPL
#define ALLC_LIST_IMPL
#endif

// List Item (Internal) {{{1
// =========================

/**
 * An item in the list.
 *
 * This is a wrapper around every element in the list. This is why it is
 * important to manage elements only with the provided `allc_list_*` functions,
 * otherwise the list might behave unexpectedly.
 **/
typedef struct allc_listitem_s {
  struct allc_listitem_s *next;
  struct allc_listitem_s *prev;
  size_t size;
  unsigned char data[];
} allc_listitem_t;

typedef allc_listitem_t ListItem;

// List {{{1
// =========

typedef struct allc_list_s {
  size_t length;
  allc_listitem_t *head;
  allc_listitem_t *last;
  allc_allocator_t allocator;
} allc_list_t;

typedef allc_list_t List;

// List - Constructing and Destructing {{{2
// ----------------------------------------

/**
 *  Create a new, empty list.
 *
 *  The list must be freed with `allc_list_delete` once it is no longer used.
 **/
allc_list_t allc_list_new(allc_allocator_t allocator);

/* Delete the list and free all its items. */
void allc_list_delete(allc_list_t *self);

// List Operations {{{2
// --------------------

/* Add element to the end of the list. */
void allc_list_append(allc_list_t *self, size_t size, void *elem);

/* Insert element at the given index. */
void allc_list_insert(allc_list_t *self, ssize_t i, size_t size, void *elem);

/* Extend list with elements from another list. */
void allc_list_extend(allc_list_t *self, allc_list_t *other);

/* Reverse the order of elements in the list. */
void allc_list_reverse(allc_list_t *self);

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
 * If `self` is NULL or index is out of bounds, return NULL.
 **/
void *allc_list_at(allc_list_t *self, ssize_t i);

/* Move element from index i to index j. */
void allc_list_move(allc_list_t *self, ssize_t i, ssize_t j);

/* Swap elements at indices i and j. */
void allc_list_swap(allc_list_t *self, ssize_t i, ssize_t j);

/* Remove the element at the given index. */
void allc_list_remove(allc_list_t *self, ssize_t i);

/* Iterate over the elements in the list. */
void *allc_list_iter_forw(void *elem);
void *allc_list_iter_back(void *elem);

// Macros {{{1
// -----------

#define ALLC_LIST_FOREACH(T, elem, list, body)                                 \
  do {                                                                         \
    T *elem = (T *)allc_list_at(list, 0);                                      \
    while (elem != NULL) {                                                     \
      body;                                                                    \
      elem = (T *)allc_list_iter_forw(elem);                                   \
    }                                                                          \
  } while (0)

#define ALLC_LIST_FOREACH_REVERSE(T, elem, list, body)                         \
  do {                                                                         \
    T *elem = (T *)allc_list_at(list, -1);                                     \
    while (elem != NULL) {                                                     \
      body;                                                                    \
      elem = (T *)allc_list_iter_back(elem);                                   \
    }                                                                          \
  } while (0)

#endif // ALLC_LIST__GUARD

/****************************************************************************/

#ifndef ALLC_LIST_IMPL__GUARD
#define ALLC_LIST_IMPL__GUARD
#else
#undef ALLC_LIST_IMPL
#endif

#ifdef ALLC_LIST_IMPL

// Internal Helper Functions {{{1
// ===============================

static allc_listitem_t *allc_listitem_new(allc_allocator_t allocator, size_t size) {
  allc_listitem_t *p = allc_allocator_alloc(allocator, size + sizeof(allc_listitem_t));
  p->next = NULL;
  p->prev = NULL;
  p->size = size;
  return p;
}

static void allc_listitem_delete(allc_allocator_t allocator, allc_listitem_t *p) {
  allc_allocator_free(allocator, p);
}

static allc_listitem_t *allc_listitem_jump(allc_listitem_t *self, ssize_t n) {
  allc_listitem_t *p = self;
  if (n < 0) {
    for (; p != NULL && n != 0; n++) {
      p = p->prev;
    }
  } else {
    for (; p != NULL && n != 0; n--) {
      p = p->next;
    }
  }
  return p;
}

static void allc_listitem_detach(allc_listitem_t *self) {
  if (self->prev != NULL) {
    self->prev->next = self->next;
  }
  if (self->next != NULL) {
    self->next->prev = self->prev;
  }
}

static void allc_listitem_attach_before(allc_listitem_t *self, allc_listitem_t *other) {
  if (other == NULL) {
    return;
  }
  if (other->prev != NULL) {
    other->prev->next = self;
  }
  self->next = other;
  self->prev = other->prev;
  other->prev = self;
}

static void allc_listitem_attach_after(allc_listitem_t *self, allc_listitem_t *other) {
  if (other == NULL) {
    return;
  }
  if (other->next != NULL) {
    other->next->prev = self;
  }
  self->prev = other;
  self->next = other->next;
  other->next = self;
}

// Setting `p = last` is a step back itself.
#define ALLC_LIST_GET_ITEM_AT(item, list, i)                                   \
    do {                                                                       \
        ssize_t idx = (i);                                                     \
        if (idx < 0) {                                                         \
            idx += (ssize_t)(list)->length;                                    \
        }                                                                      \
        if (idx < 0 || idx >= (ssize_t)(list)->length) {                      \
            item = NULL;                                                       \
        } else {                                                               \
            item = idx < (ssize_t)(list)->length / 2 ?                        \
                   allc_listitem_jump((list)->head, idx) :                    \
                   allc_listitem_jump((list)->last, idx - (ssize_t)(list)->length + 1); \
        }                                                                      \
    } while(0)

// List Implementation {{{1
// ========================

allc_list_t allc_list_new(allc_allocator_t allocator)
{
    return (allc_list_t) {
        .length = 0,
        .head = NULL,
        .last = NULL,
        .allocator = allocator,
    };
}

void allc_list_delete(allc_list_t *self)
{
    if (self == NULL) return;
    
    for (allc_listitem_t *q, *p = self->head; p != NULL; p = q) {
        q = p->next;
        allc_listitem_delete(self->allocator, p);
    }
    
    self->head = NULL;
    self->last = NULL;
    self->length = 0;
}

// List Operations {{{2
// --------------------

void allc_list_append(allc_list_t *self, size_t size, void *elem)
{
    if (self == NULL || elem == NULL) return;
    
    allc_listitem_t *item = allc_listitem_new(self->allocator, size);
    memcpy(item->data, elem, size);
    
    if (self->last == NULL) {
        // First element
        self->head = self->last = item;
    } else {
        allc_listitem_attach_after(item, self->last);
        self->last = item;
    }
    
    self->length += 1;
}

void allc_list_insert(allc_list_t *self, ssize_t i, size_t size, void *elem)
{
    if (self == NULL || elem == NULL) return;
    
    // Handle special cases
    if (i >= (ssize_t)self->length) {
        // Insert at or beyond the end - append
        return allc_list_append(self, size, elem);
    }
    
    if (i < 0 && -i > (ssize_t)self->length) {
        // Negative index beyond start - insert at beginning (index 0)
        i = 0;
    }
    
    allc_listitem_t *other;
    ALLC_LIST_GET_ITEM_AT(other, self, i);
    
    if (other == NULL) {
        // This shouldn't happen with the checks above, but handle gracefully
        return allc_list_append(self, size, elem);
    }
    
    allc_listitem_t *item = allc_listitem_new(self->allocator, size);
    memcpy(item->data, elem, size);
    
    allc_listitem_attach_before(item, other);
    
    if (other == self->head) {
        self->head = item;
    }
    
    self->length += 1;
}

void allc_list_extend(allc_list_t *self, allc_list_t *other)
{
    if (self == NULL || other == NULL) return;
    
    for (allc_listitem_t *p = other->head; p != NULL; p = p->next) {
        allc_list_append(self, p->size, p->data);
    }
}

void allc_list_reverse(allc_list_t *self)
{
    if (self == NULL || self->length <= 1) return;
    
    allc_listitem_t *current = self->head;
    allc_listitem_t *temp = NULL;
    
    // Swap head and last
    self->last = self->head;
    
    while (current != NULL) {
        temp = current->prev;
        current->prev = current->next;
        current->next = temp;
        current = current->prev;
    }
    
    if (temp != NULL) {
        self->head = temp->prev;
    }
}

// Element Operations {{{2
// -----------------------

void *allc_list_at(allc_list_t *self, ssize_t i)
{
    if (self == NULL) return NULL;
    
    allc_listitem_t *p;
    ALLC_LIST_GET_ITEM_AT(p, self, i);
    return p == NULL ? NULL : p->data;
}

void allc_list_move(allc_list_t *self, ssize_t i, ssize_t j)
{
    if (self == NULL || i == j || self->length <= 1) return;
    
    allc_listitem_t *item_i;
    ALLC_LIST_GET_ITEM_AT(item_i, self, i);
    if (item_i == NULL) return;
    
    // Remove item from current position
    if (item_i == self->head) {
        self->head = item_i->next;
    }
    if (item_i == self->last) {
        self->last = item_i->prev;
    }
    allc_listitem_detach(item_i);
    self->length--;
    
    // Insert at new position
    item_i->next = item_i->prev = NULL;
    
    if (j >= (ssize_t)self->length) {
        // Insert at end
        if (self->last != NULL) {
            allc_listitem_attach_after(item_i, self->last);
            self->last = item_i;
        } else {
            self->head = self->last = item_i;
        }
    } else {
        allc_listitem_t *item_j;
        ALLC_LIST_GET_ITEM_AT(item_j, self, j);
        if (item_j != NULL) {
            allc_listitem_attach_before(item_i, item_j);
            if (item_j == self->head) {
                self->head = item_i;
            }
        } else {
            // Insert at end
            if (self->last != NULL) {
                allc_listitem_attach_after(item_i, self->last);
                self->last = item_i;
            } else {
                self->head = self->last = item_i;
            }
        }
    }
    
    self->length++;
}

void allc_list_swap(allc_list_t *self, ssize_t i, ssize_t j)
{
    if (self == NULL || i == j) return;
    
    void *elem_i = allc_list_at(self, i);
    void *elem_j = allc_list_at(self, j);
    
    if (elem_i == NULL || elem_j == NULL) return;
    
    allc_listitem_t *item_i, *item_j;
    ALLC_LIST_GET_ITEM_AT(item_i, self, i);
    ALLC_LIST_GET_ITEM_AT(item_j, self, j);
    
    if (item_i == NULL || item_j == NULL) return;
    
    // Only swap if both items have the same size
    if (item_i->size != item_j->size) {
        return; // Cannot swap items of different sizes safely
    }
    
    // Swap the data contents
    unsigned char *temp = allc_allocator_alloc(self->allocator, item_i->size);
    memcpy(temp, item_i->data, item_i->size);
    memcpy(item_i->data, item_j->data, item_j->size);
    memcpy(item_j->data, temp, item_i->size);
    
    allc_allocator_free(self->allocator, temp);
}

void allc_list_remove(allc_list_t *self, ssize_t i)
{
    if (self == NULL) return;
    
    allc_listitem_t *p;
    ALLC_LIST_GET_ITEM_AT(p, self, i);
    if (p == NULL) return;
    
    if (self->head == p) { 
        self->head = p->next; 
    }
    if (self->last == p) { 
        self->last = p->prev; 
    }
    
    allc_listitem_detach(p);
    self->length -= 1;
    allc_listitem_delete(self->allocator, p);
}

void *allc_list_iter_forw(void *elem)
{
    if (elem == NULL) return NULL;
    
    allc_listitem_t *item = (allc_listitem_t *)((unsigned char *)elem - sizeof(allc_listitem_t));
    return item->next == NULL ? NULL : item->next->data;
}

void *allc_list_iter_back(void *elem)
{
    if (elem == NULL) return NULL;
    
    allc_listitem_t *item = (allc_listitem_t *)((unsigned char *)elem - sizeof(allc_listitem_t));
    return item->prev == NULL ? NULL : item->prev->data;
}

// }}}1

#endif // ALLC_LIST_IMPL

// vim: ai et ts=4 sw=0 fdl=99 fdm=marker
